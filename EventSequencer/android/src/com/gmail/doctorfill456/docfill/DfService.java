package com.gmail.doctorfill456.docfill;

import android.content.Context;
import android.content.Intent;
import org.qtproject.qt5.android.bindings.QtService;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;

public class DfService extends QtService
{
    public static void startMyService(Context ctx) {
        ctx.startForegroundService(new Intent(ctx, DfService.class));
    }

    private static final int ONGOING_NOTIFICATION_ID = 10;

    private void createChannel() {
        NotificationChannel channel =
            new NotificationChannel("CHANNEL_BACKGROUND_WORKER", "Background Worker",
                                    NotificationManager.IMPORTANCE_LOW);
        NotificationManager nm = getSystemService(NotificationManager.class);
        nm.createNotificationChannel(channel);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        createChannel();

        Notification notification =
            new Notification.Builder(this, "CHANNEL_BACKGROUND_WORKER")
            .setContentTitle("DocFill")
            .setContentText("Processing...")
            .setSmallIcon(R.drawable.ic_stat_docfill)
            .build();

        startForeground(ONGOING_NOTIFICATION_ID, notification);

        return START_STICKY;
    }
}
