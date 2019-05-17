package com.example.bike;


import android.support.v4.app.FragmentActivity;
import android.os.Bundle;

import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

public class MapsActivity extends FragmentActivity implements OnMapReadyCallback {


    private GoogleMap mMap;
    double LATITUDE;
    double LONGITUDE;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);

        Bundle data = this.getIntent().getExtras().getBundle("location");
        this.LATITUDE = data.getDouble("lat");
        this.LONGITUDE = data.getDouble("lon");

        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
    }


    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;

        LatLng bike = new LatLng(LATITUDE, LONGITUDE);
        mMap.addMarker(new MarkerOptions().position(bike).title("Your bike location"));
        CameraUpdate cu = CameraUpdateFactory.newLatLngZoom(bike, 12f);

        mMap.animateCamera(cu);
    }
}
