package com.gmail.doctorfill456.docfill;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.support.v4.app.NotificationCompat;
import android.util.Log;

import org.qtproject.qt5.android.bindings.QtService;

public class DfService extends QtService
{
    public static void startMyService(Context ctx) {
        // Would prefer startForegroundService, but not doing so for compat reasons.
        ctx.startService(new Intent(ctx, DfService.class));
    }

    private static final int ONGOING_NOTIFICATION_ID = 10;
    private static String CHANNEL_ID = "CHANNEL_BACKGROUND_WORKER";

    private static String CANCEL_WORKER_ACTION = "com.gmail.doctorfill456.docfill.action.CANCEL_WORKER";

    private BroadcastReceiver notificationActionReceiver_ = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(CANCEL_WORKER_ACTION)) {
                Log.i("DOCFILL", "Cancel not yet implemented.");
            }
        }
    };

    private void createNotificationChannel() {
        // Create the NotificationChannel, but only on API 26+ because
        // the NotificationChannel class is new and not in the support library
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            CharSequence name = "Background Worker";
            //String description = getString(R.string.channel_description);
            int importance = NotificationManager.IMPORTANCE_LOW;
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, name, importance);
            //channel.setDescription(description);
            // Register the channel with the system; you can't change the importance
            // or other notification behaviors after this
            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            notificationManager.createNotificationChannel(channel);
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        createNotificationChannel();

        Intent cancelIntent = new Intent();
        cancelIntent.setAction(CANCEL_WORKER_ACTION);
        PendingIntent cancelBroadcast =
                PendingIntent.getBroadcast(this, 0, cancelIntent, 0);

        NotificationCompat.Builder notificationBuilder =
                new NotificationCompat.Builder(this, CHANNEL_ID)
                        .setContentTitle("DocFill")
                        .setContentText("Processing...")
                        .setSmallIcon(R.drawable.ic_stat_docfill)
                        .addAction(R.drawable.ic_stat_docfill, "Cancel", cancelBroadcast);

        startForeground(ONGOING_NOTIFICATION_ID, notificationBuilder.build());

        return START_STICKY;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        registerReceiver(notificationActionReceiver_, new IntentFilter(CANCEL_WORKER_ACTION));
    }

    @Override
    public void onDestroy() {
        unregisterReceiver(notificationActionReceiver_);
        super.onDestroy();
    }

}
