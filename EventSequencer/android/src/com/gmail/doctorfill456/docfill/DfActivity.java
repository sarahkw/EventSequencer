package com.gmail.doctorfill456.docfill;

import android.os.Bundle;
import org.qtproject.qt5.android.bindings.QtActivity;

public class DfActivity extends QtActivity {

    // When there's a foreground service running, Android doesn't kill
    // us, so we're left hanging. So, when the user tries to start the
    // activity again, it doesn't start. So, terminate our process as
    // a workaround. m_quitApp is set according to how Qt 5.11.1
    // works.
    private boolean m_quitApp = true;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        m_quitApp = true;
        super.onCreate(savedInstanceState);
    }

    @Override
    public Object onRetainNonConfigurationInstance() {
        m_quitApp = false;
        return super.onRetainNonConfigurationInstance();
    }

    @Override
    protected void onDestroy() {
        if (m_quitApp) {
            // I guess we're just going to simulate android killing
            // us. Unfortunately, calling super.onDestroy() will hang.
            android.os.Process.killProcess(android.os.Process.myPid());
        }
        super.onDestroy();
    }
}
