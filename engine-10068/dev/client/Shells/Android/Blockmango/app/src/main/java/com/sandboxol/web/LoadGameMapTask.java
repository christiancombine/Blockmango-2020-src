package com.sandboxol.web;

import android.os.AsyncTask;
import android.util.Log;

import com.google.gson.Gson;
import com.sandboxol.blockmango.UserInfo;

/**
 * Created by Mr.Luo on 2017/2/10.
 */

public class LoadGameMapTask extends AsyncTask<Void, Void, HttpResponse<Dispatch>> {


    private OnLoadGameMapListener mOnLoadGameMapListener;

    public LoadGameMapTask(OnLoadGameMapListener onLoadGameMapListener) {
        this.mOnLoadGameMapListener = onLoadGameMapListener;
    }

    @Override
    protected HttpResponse<Dispatch> doInBackground(Void... params) {
        HttpResponse<MiniGameToken> response = Web.loadMiniGameToken(UserInfo.newInstance().userId, UserInfo.newInstance().token, UserInfo.newInstance().gameType);
        Log.e("Web:Enter.GameToken", new Gson().toJson(response));
        if (response.getCode() == 1 && response.getData() != null) {
            return webConnection(response.getData());
        } else {
            HttpResponse<Dispatch> httpResponse = new HttpResponse<>();
            httpResponse.setCode(response.getCode());
            return httpResponse;
        }
    }

    private HttpResponse<Dispatch> webConnection(MiniGameToken token) {
        HttpResponse<Dispatch> response = new HttpResponse<>();
        if (token != null) {
            HttpResponse<MiniGameMap> httpResponse = Web.loadMiniGameMap(token.getDispUrl());
            Log.e("Web:Enter.GameMap", new Gson().toJson(httpResponse));
            response.setCode(httpResponse.getCode());
            if (httpResponse.getCode() == 1) {
                Dispatch dispatch = new Dispatch();
                dispatch.token = token.getToken();
                dispatch.dispUrl = token.getDispUrl();
                dispatch.signature = token.getSignature();
                dispatch.timestamp = token.getTimestamp();
                dispatch.mapId = httpResponse.getData().getMapName();
                dispatch.mapName = httpResponse.getData().getMapName();
                dispatch.mapUrl = httpResponse.getData().getDownloadUrl();
                dispatch.name = UserInfo.newInstance().nickName;
                dispatch.userId = UserInfo.newInstance().userId;
                dispatch.gameType = UserInfo.newInstance().gameType;
                response.setData(dispatch);
                Log.e("Web:Enter.GameDispatch", new Gson().toJson(response));
                return response;
            }
            return response;
        }
        response.setCode(0);
        return response;
    }

    @Override
    protected void onPostExecute(HttpResponse<Dispatch> response) {
        if (mOnLoadGameMapListener != null) {
            mOnLoadGameMapListener.onLoadGameMap(response.getCode(), response.getData());
        }
    }

    public interface OnLoadGameMapListener {
        void onLoadGameMap(int code, Dispatch dispatch);
    }
}
