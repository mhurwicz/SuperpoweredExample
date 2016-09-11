package com.superpowered.example;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.media.AudioManager;
import android.content.Context;
import android.content.res.AssetFileDescriptor;
import java.io.IOException;
import android.os.Build;
import android.widget.Button;
import android.view.View;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    boolean playing = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Get the device's sample rate and buffer size to enable low-latency Android audio output, if available.
        String samplerateString = null, buffersizeString = null;
        if (Build.VERSION.SDK_INT >= 17) {
            AudioManager audioManager = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
            samplerateString = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
            buffersizeString = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        }
        if (samplerateString == null) samplerateString = "44100";
        if (buffersizeString == null) buffersizeString = "512";

        // Files under res/raw are not zipped, just copied into the APK. Get the offset and length to know where our files are located.
        AssetFileDescriptor fd0 = getResources().openRawResourceFd(R.raw.lycka);
        int fileAoffset = (int)fd0.getStartOffset(), fileAlength = (int)fd0.getLength();
        try {
            fd0.getParcelFileDescriptor().close();
        } catch (IOException e) {
            android.util.Log.d("", "Close error.");
        }

        // Arguments: path to the APK file, offset and length of the two resource files, sample rate, audio buffer size.
        SuperpoweredExample(Integer.parseInt(samplerateString), Integer.parseInt(buffersizeString), getPackageResourcePath(), fileAoffset, fileAlength);

        final Button pauseButton = (Button) findViewById(R.id.pause_button);
        assert pauseButton != null;
        pauseButton.setOnClickListener(this);

        final Button playButton1 = (Button) findViewById(R.id.play_button_1);
        assert playButton1 != null;
        playButton1.setOnClickListener(this);

        final Button playButton2 = (Button) findViewById(R.id.play_button_2);
        assert playButton2 != null;
        playButton2.setOnClickListener(this);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onClick(View v) {
        switch(v.getId()){
            case R.id.pause_button:
                if(playing) pause();
                else play();
                playing = !playing;
                break;
            case R.id.play_button_1: {
                AssetFileDescriptor fd = getResources().openRawResourceFd(R.raw.lycka);
                int fileOffset = (int)fd.getStartOffset(), fileLength = (int)fd.getLength();
                try {
                    fd.getParcelFileDescriptor().close();
                } catch (IOException e) {
                    Log.d("", "Close error.");
                }
                open(getPackageResourcePath(), fileOffset, fileLength);
                playing = false;
                break;
            }
            case R.id.play_button_2: {
                AssetFileDescriptor fd = getResources().openRawResourceFd(R.raw.nuyorica);
                int fileOffset = (int)fd.getStartOffset(), fileLength = (int)fd.getLength();
                try {
                    fd.getParcelFileDescriptor().close();
                } catch (IOException e) {
                    Log.d("", "Close error.");
                }
                open(getPackageResourcePath(), fileOffset, fileLength);
                playing = false;
                break;
            }
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        stop();
    }

    @Override
    protected void onResume() {
        super.onResume();
        start();
    }

    private native void SuperpoweredExample(int samplerate, int buffersize, String apkPath, int fileAoffset, int fileAlength);
    private native void open(String path, int fileOffset, int fileLength);
    private native void play();
    private native void pause();

    private native void onBackground();
    private native void onForeground();
    private native void stop();
    private native void start();

    static {
        System.loadLibrary("SuperpoweredJNI");
    }
}
