package com.example.bike;

import com.example.bike.models.Data;
import com.example.bike.models.Response;
import com.example.bike.models.Unlocking;

import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.Field;
import retrofit2.http.FormUrlEncoded;
import retrofit2.http.GET;
import retrofit2.http.POST;
import retrofit2.http.Path;

public interface Api {

    @GET("/api/data")
    Call<Data> getData();

    @GET("/api/call/unlock")
    Call<Unlocking> unlock();

    @GET("/api/call/lock")
    Call<Unlocking> lock();

}
