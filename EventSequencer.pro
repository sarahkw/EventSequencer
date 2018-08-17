TEMPLATE = subdirs

SUBDIRS += \
    eventsequencerlib \
    EventSequencer

# I don't know why, but I can't do:
#
#  !android {
#      SUBDIRS += eventsequencertest
#  }
#
# Doing that will include the tests on Android anyway.

EVENTSEQUENCERTEST_STR = "eventsequencertest"
android {
    # Couldn't find a way to look at test outputs on Android. If the tests even
    # run...
    EVENTSEQUENCERTEST_STR = ""
}
SUBDIRS += $$EVENTSEQUENCERTEST_STR
