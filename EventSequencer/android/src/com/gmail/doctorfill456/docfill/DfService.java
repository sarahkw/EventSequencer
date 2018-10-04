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
import android.support.v4.app.NotificationManagerCompat;

import org.qtproject.qt5.android.bindings.QtService;

public class DfService extends QtService
{
    private static native void notificationCancelWorker();

    // We could have a status update when we're not working. Because clearing the status would be
    // a status update.
    private boolean ongoingNotificationVisible_ = false;

    // Called from Android thread
    public void workerStatusChanged(String fileName, String statusText) {
        if (ongoingNotificationVisible_) {
            NotificationManagerCompat notificationManager = NotificationManagerCompat.from(this);
            notificationManager.notify(ONGOING_NOTIFICATION_ID, ongoingNotificationBuilder(fileName, statusText).build());
        }
    }

    // Called from Android thread
    public void workerStatusResult(String fileName, String statusText) {
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle(String.format("Export Result: %s", fileName))
                .setContentText(statusText)
                .setSmallIcon(R.drawable.ic_stat_docfill);
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(this);
        notificationManager.notify(RESULT_NOTIFICATION_ID, builder.build());
    }

    // Called from Qt thread
    public static void androidStartService(Context ctx) {
        // Would prefer startForegroundService, but not doing so for compat reasons.
        ctx.startService(new Intent(ctx, DfService.class));
    }

    // Called from Android thread
    public void androidStopService() {
        ongoingNotificationVisible_ = false;
        stopForeground(true);
        stopSelf();
    }

    private static final int ONGOING_NOTIFICATION_ID = 10;
    private static final int RESULT_NOTIFICATION_ID = 11;
    private static String CHANNEL_ID = "CHANNEL_BACKGROUND_WORKER";

    private static String CANCEL_WORKER_ACTION = "com.gmail.doctorfill456.docfill.action.CANCEL_WORKER";

    private BroadcastReceiver notificationActionReceiver_ = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getAction().equals(CANCEL_WORKER_ACTION)) {
                notificationCancelWorker();
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

    private NotificationCompat.Builder ongoingNotificationBuilder(String title, String text) {
        return new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle(title == null ? "DocFill" : String.format("Exporting: %s", title))
                .setContentText(text == null ? "Processing..." : text)
                .setSmallIcon(R.drawable.ic_stat_docfill)
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .addAction(0, "Cancel", cancelBroadcast_);
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        createNotificationChannel();

        NotificationCompat.Builder notificationBuilder =
                ongoingNotificationBuilder(null, null);

        startForeground(ONGOING_NOTIFICATION_ID, notificationBuilder.build());
        ongoingNotificationVisible_ = true;

        return START_STICKY;
    }

    PendingIntent cancelBroadcast_;

    @Override
    public void onCreate() {
        super.onCreate();
        registerReceiver(notificationActionReceiver_, new IntentFilter(CANCEL_WORKER_ACTION));

        // Will re-use same pending intent for all status updates
        {
            Intent cancelIntent = new Intent();
            cancelIntent.setAction(CANCEL_WORKER_ACTION);
            cancelBroadcast_ =
                    PendingIntent.getBroadcast(this, 0, cancelIntent, 0);
        }
    }

    @Override
    public void onDestroy() {
        unregisterReceiver(notificationActionReceiver_);
        super.onDestroy();
    }
}
