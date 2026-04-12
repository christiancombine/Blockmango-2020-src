package com.sandboxol.web;

import com.sandboxol.blockmango.BuildConfig;
import com.sandboxol.blockmango.UserInfo;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.TimeUnit;

import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

/**
 * Created by Administrator on 2018/2/2 0002.
 */

public class Web {

    private static GameApi miniGameApi = RetrofitUtils.createMiniGameApi(GameApi.class);


    public static HttpResponse<MiniGameToken> loadMiniGameToken(long userId, String token, String gameType) {
        try {
            //Response<HttpResponse<MiniGameToken>> response = miniGameApi.loadMiniGameToken(userId, gameType, token).execute();
            Response<HttpResponse<MiniGameToken>> response = miniGameApi.loadMiniGameToken(userId, UserInfo.newInstance().targetId, UserInfo.newInstance().engineVer, gameType , "CN", token ).execute();
            if (response.isSuccessful() && response.code() == 200 && response.body().getCode() == 1) {
                return response.body();
            } else {
                HttpResponse<MiniGameToken> httpResponse = new HttpResponse<>();
                httpResponse.setCode(response.code() * 10);
                return httpResponse;
            }
        } catch (Exception e) {
            HttpResponse<MiniGameToken> httpResponse = new HttpResponse<>();
            String str = e.toString();
            int code = 0;
            if (str.contains("Timeout")) {
                code = 1000008;
            }
            if (str.contains("UnknownHost")) {
                code = 1000009;
            }
            httpResponse.setCode(code);

            return httpResponse;
        }
    }

    public static HttpResponse<MiniGameMap> loadMiniGameMap(String dispUrl) {
        try {
            final okhttp3.OkHttpClient okHttpClient = new okhttp3.OkHttpClient.Builder().connectTimeout(2, TimeUnit.SECONDS).readTimeout(8, TimeUnit.SECONDS).build();
            Retrofit retrofit = new Retrofit.Builder().client(okHttpClient).baseUrl(dispUrl).addCallAdapterFactory(new ErrorHandlingCallAdapter.ErrorHandlingCallAdapterFactory()).addConverterFactory(GsonConverterFactory.create()).build();
            Response<HttpResponse<MiniGameMap>> response =  retrofit.create(GameApi.class).loadMiniGameMap(UserInfo.newInstance().mapId , UserInfo.newInstance().gameType, UserInfo.newInstance().engineVer).execute();
            if (response.isSuccessful() && response.code() == 200 && response.body().getCode() == 1) {
                return response.body();
            } else {
                HttpResponse<MiniGameMap> httpResponse = new HttpResponse<>();
                httpResponse.setCode(response.code() * 10);
                return httpResponse;
            }
        } catch (Exception e) {
            HttpResponse<MiniGameMap> httpResponse = new HttpResponse<>();
            String str = e.toString();
            int code = 0;
            if (str.contains("Timeout")) {
                code = 1000008;
            }
            if (str.contains("UnknownHost")) {
                code = 1000009;
            }
            httpResponse.setCode(code);

            return httpResponse;
        }
    }


    public static HttpResponse<Dispatch> miniGameDispatcher(long userId, long targetId, String nickName, String token, long ever, String dispUrl, List<String> ips) {
        HttpResponse<Dispatch> httpResponse;
        try {
            final okhttp3.OkHttpClient okHttpClient = new okhttp3.OkHttpClient.Builder().connectTimeout(2, TimeUnit.SECONDS).readTimeout(8, TimeUnit.SECONDS).build();
            Retrofit retrofit = new Retrofit.Builder().client(okHttpClient).baseUrl(dispUrl)
                    .addCallAdapterFactory(new ErrorHandlingCallAdapter.ErrorHandlingCallAdapterFactory()).addConverterFactory(GsonConverterFactory.create()).build();
            Map<String, Object> map = new HashMap<>();
            map.put("clz", 0);
            map.put("rid", 1001);
            map.put("name", nickName);
            map.put("ever", ever);
            map.put("pioneer", true);
            map.put("targetId", targetId);
            map.put("excludeIps", ips);

            Response<HttpResponse<Dispatch>> response = retrofit.create(GameApi.class).newMiniGameDispatcher(map, userId, token).execute();
            if (response.isSuccessful() && response.code() == 200 && response.body() != null) {
                return response.body();
            } else {
                httpResponse = new HttpResponse<>();
                httpResponse.setCode(response.code());
                return httpResponse;
            }
        } catch (Exception e) {

            httpResponse = new HttpResponse<>();
            String str = e.toString();
            int code = 0;
            if (str.contains("Timeout")) {
                code = 1000008;
            }
            if (str.contains("UnknownHost")) {
                code = 1000009;
            }
            httpResponse.setCode(code);
            return httpResponse;
        }
    }

}
