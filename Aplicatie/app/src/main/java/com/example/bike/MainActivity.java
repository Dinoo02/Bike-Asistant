package com.example.bike;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;
import com.example.bike.models.Data;
import com.example.bike.models.Unlocking;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;


public class MainActivity extends AppCompatActivity {

    //special variabiles
    private static Button button;
    private static ImageView image_view;

    //simple variabiles
    public static final String SHARED_PREFS = "sharedPrefs";
    private boolean PRESSED = false;
    private String CLOSE = "Apasa sa Inchizi";
    private String OPEN = "Apasa sa Deschizi";
    private int CURRENT_IMAGE = 2;

    //images array
    private int images[] = {R.drawable.open, R.drawable.close};
    
    //api
    private Api api;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.api = RetrofitClient
                .getmInstance()
                .getApi();

        changePhotoWhenPressed();

        //loadData();
    }

    private void setButtonText() {
        PRESSED = !PRESSED;

        if (PRESSED) {
            button.setText(OPEN);
            sendDataLock();
        }
         else {
            button.setText(CLOSE);
            sendDataUnlock();
        }
    }

    private void sendDataUnlock(){

        Call<Unlocking> call = this.api.unlock();

        call.enqueue(new Callback<Unlocking>() {
            @Override
            public void onResponse(Call<Unlocking> call, Response<Unlocking> response) {
            }
            @Override
            public void onFailure(Call<Unlocking> call, Throwable t) {
            }
        });
    }
    private void sendDataLock(){
        Call<Unlocking> call = this.api.lock();

        call.enqueue(new Callback<Unlocking>() {
            @Override
            public void onResponse(Call<Unlocking> call, Response<Unlocking> response) {
            }
            @Override
            public void onFailure(Call<Unlocking> call, Throwable t) {
            }
        });
    }
    public void openMapsActivity(View view){

        final MainActivity that = this;
        Call<Data> call = this.api.getData();
        call.enqueue(new Callback<Data>() {
            @Override
            public void onResponse(Call<Data> call, Response<Data> response) {

                Bundle data = new Bundle();

                data.putDouble("lat", response.body().lat);
                data.putDouble("lon", response.body().lon);

                Intent intent = new Intent(that, MapsActivity.class);
                intent.putExtra("location", data);

                startActivity(intent);

                overridePendingTransition(R.anim.slide_in_right, R.anim.slide_to_left);

            }

            @Override
            public void onFailure(Call<Data> call, Throwable t) {
                Toast.makeText(MainActivity.this, t.getMessage(), Toast.LENGTH_SHORT).show();
            }
        });
    }

    public void changePhotoWhenPressed() {

        image_view = (ImageView) findViewById(R.id.imageView);
        button = (Button) findViewById(R.id.button2);
        button.setOnClickListener(

                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        setButtonText();

                        CURRENT_IMAGE++;
                        CURRENT_IMAGE %= images.length;
                        image_view.setImageResource(images[CURRENT_IMAGE]);
                    }
                }
        );
    }

}