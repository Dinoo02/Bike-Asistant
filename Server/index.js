const net = require('net');
const express = require('express');
const fs = require('fs');

const Nexmo = require('nexmo');

const TCP_PORT = 443;
const TCP_HOST = '0.0.0.0';
const PING_TIME = 5000;

const HTTP_PORT = 80;
const HTTP_HOST = '0.0.0.0';

const NEXMO_PHONE = '40724702368';

const nexmo = new Nexmo({
    apiKey: '5054d60e',
    apiSecret: 'BWSEEOvXSQ00C7mM',
    applicationId: 'e6617278-82ab-4dd4-aa6f-9b60db62679b',
    privateKey: './nexmo-private.key',
});

const ncco = [
    {
        action: 'talk',
        voiceName: 'Salli',
        text: 'Hi there! Bike Assist on the phone. We noticed that your bike lock was open without your permission. ',
    },
];

for(let i = 0; i < 7; i++) {
    ncco[0].text += 'Your bike could be stollen! ';
}

let server = net.createServer();
let app = express();

/**
 * @type { net.Socket }
 */
let globalSocket = null;

let currentData = {};
try {
    currentData = JSON.parse(fs.readFileSync('./device-data.json'));
} catch (ex) {
    console.log("Could not recover device data.");
}

/**
 * @param { net.Socket } socket 
 */
function socketReady(socket) {
    globalSocket = socket;

    socket.timerId = setInterval(_ => {
        socket.write('PING@');
    }, PING_TIME);

    socket.on('data', (data) => {
        let dt = {};

        let chunks = data.toString().split('\n');

        for (let chunk of chunks) {
            if (chunk.indexOf("CGPSINF:") >= 0) {
                let gpsData = chunk.split(',');

                dt.lat = gpsData[1];
                dt.lon = gpsData[2];
            } else if (chunk.indexOf("LOCKBRK") >= 0) {
                console.log("Lock break detected. Sending phone notification.");
                
                nexmo.calls.create(
                    {
                        to: [{ type: 'phone', number: NEXMO_PHONE }],
                        from: { type: 'phone', number: NEXMO_PHONE },
                        ncco,
                    },
                    (err, result) => {
                        console.log(err || result);
                    },
                );
            }
        }

        currentData = Object.assign(currentData, dt);

        fs.writeFile("./device-data.json", JSON.stringify(currentData), (err) => {
            if (!!err) console.log("Could not write updated data to persistent storage.");
        });

        console.log("Updated current device data", currentData);
    });

    socket.on('close', () => {
        clearInterval(socket.timerId);

        globalSocket = null;
        console.log(`Closed socket: ${socket.remoteAddress}:${socket.remotePort}.`);
    });
}

server.on('connection', (socket) => {
    console.log(`New socket: ${socket.remoteAddress}:${socket.remotePort}.`);
    setTimeout(_ => {
        socket.write("@");

        socketReady(socket);
    }, 1000);
});

app.get('/api/call/:action', (request, response) => {
    if (globalSocket == null) {
        response.json({ command: request.params['action'], ok: false, reason: 'bike.disconnected' });
    } else {
        globalSocket.write(request.params['action'] + "@");
        response.json({ command: request.params['action'], ok: true })
    }
});

app.get('/api/data', (request, response) => {
    if (globalSocket == null) {
        response.send({ ...currentData, ok: false, reason: 'bike.disconnected' });
    } else {
        response.send({ ...currentData, ok: true });
    }
});

server.listen(TCP_PORT, TCP_HOST, () => {
    console.log(`TCP server listening on ${TCP_HOST}:${TCP_PORT}.`);
});

app.listen(HTTP_PORT, HTTP_HOST, () => {
    console.log(`HTTP server listening on ${HTTP_HOST}:${HTTP_PORT}.`);
});