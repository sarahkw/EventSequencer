import QtQuick 2.0
import QtTest 1.11

Item {
    ZoomLogic {
        id: zl
    }

    TestCase {
        function test_numberToScale() {
            compare(zl.numberToScale(10      ) , 10      , '10     ');
            compare(zl.numberToScale(50      ) , 10      , '50     ');
            compare(zl.numberToScale(100     ) , 100     , '100    ');
            compare(zl.numberToScale(500     ) , 100     , '500    ');
            compare(zl.numberToScale(1000    ) , 1000    , '1000   ');
            compare(zl.numberToScale(10000   ) , 10000   , '10000  ');
            compare(zl.numberToScale(50000   ) , 10000   , '50000  ');
            compare(zl.numberToScale(100000  ) , 100000  , '100000 ');
            compare(zl.numberToScale(500000  ) , 100000  , '500000 ');
            compare(zl.numberToScale(1000000 ) , 1000000 , '1000000');
            compare(zl.numberToScale(5000000 ) , 1000000 , '5000000');
        }

        function test_nextFocusNumber_up() {
            var up = 1;

            compare(zl.nextFocusNumber(1, up), 5);
            compare(zl.nextFocusNumber(5, up), 10);

            compare(zl.nextFocusNumber(10, up), 50);
            compare(zl.nextFocusNumber(50, up), 100);
            compare(zl.nextFocusNumber(100, up), 500);
            compare(zl.nextFocusNumber(500, up), 1000);
        }
        function test_nextFocusNumber_dn() {
            var dn = -1;

            compare(zl.nextFocusNumber(10, dn), 5);
            compare(zl.nextFocusNumber(5, dn), 1);

            compare(zl.nextFocusNumber(1000, dn), 500);
            compare(zl.nextFocusNumber(500, dn), 100);
            compare(zl.nextFocusNumber(100, dn), 50);
            compare(zl.nextFocusNumber(50, dn), 10);
        }
    }
}
