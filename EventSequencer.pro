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
# It feels like I have to try to outsmart qmake.

EVENTSEQUENCERTEST_STR = "eventsequencertest"
android {
    EVENTSEQUENCERTEST_STR = ""
}
SUBDIRS += $$EVENTSEQUENCERTEST_STR
