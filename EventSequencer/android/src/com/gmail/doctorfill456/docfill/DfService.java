package com.gmail.doctorfill456.docfill;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.support.v4.app.NotificationCompat;

import org.qtproject.qt5.android.bindings.QtService;

public class DfService extends QtService
{
    public static void startMyService(Context ctx) {
        // Would prefer startForegroundService, but not doing so for compat reasons.
        ctx.startService(new Intent(ctx, DfService.class));
    }

    private static final int ONGOING_NOTIFICATION_ID = 10;
    private static String CHANNEL_ID = "CHANNEL_BACKGROUND_WORKER";

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

        NotificationCompat.Builder notificationBuilder =
                new NotificationCompat.Builder(this, CHANNEL_ID)
                        .setContentTitle("DocFill")
                        .setContentText("Processing...")
                        .setSmallIcon(R.drawable.ic_stat_docfill);

        startForeground(ONGOING_NOTIFICATION_ID, notificationBuilder.build());

        return START_STICKY;
    }
}
