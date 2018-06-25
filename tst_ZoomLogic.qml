import QtQuick 2.0
import QtTest 1.11

Item {
    ZoomLogic {
        id: zl
    }

    TestCase {
        function test_numberToScale() {
            compare(zl.numberToScale(10      ) , 1       , '10     ');
            compare(zl.numberToScale(50      ) , 10      , '50     ');
            compare(zl.numberToScale(100     ) , 10      , '100    ');
            compare(zl.numberToScale(500     ) , 100     , '500    ');
            compare(zl.numberToScale(1000    ) , 100     , '1000   ');
            compare(zl.numberToScale(10000   ) , 1000    , '10000  ');
            compare(zl.numberToScale(50000   ) , 10000   , '50000  ');
            compare(zl.numberToScale(100000  ) , 10000   , '100000 ');
            compare(zl.numberToScale(500000  ) , 100000  , '500000 ');
            compare(zl.numberToScale(1000000 ) , 100000  , '1000000');
            compare(zl.numberToScale(5000000 ) , 1000000 , '5000000');

            compare(zl.numberToScale(20) , 10);
            compare(zl.numberToScale(40) , 10);
            compare(zl.numberToScale(60) , 10);
            compare(zl.numberToScale(80) , 10);
            compare(zl.numberToScale(800) , 100);

            compare(zl.numberToScale(1) , 0.1);
        }

        function test_nextFactor() {
            function NextFacWrap(initial, direction, division) {
                this.cur = initial;
                this.direction = direction;
                this.division = division;
            }
            NextFacWrap.prototype.next = function () {
                var ret = zl.nextFactor(this.cur, this.direction, this.division);
                this.cur = ret[0];
                return ret;
            }

            var up = 1;
            var a = new NextFacWrap(2, up, 2);
            compare(a.next(), [4, false]);
            compare(a.next(), [6, false]);
            compare(a.next(), [8, false]);
            compare(a.next(), [10, false]);
            compare(a.next(), [2, true]);
            compare(a.next(), [4, false]);
            compare(a.next(), [6, false]);

            var dn = -1;
            var a = new NextFacWrap(2, dn, 2);
            compare(a.next(), [10, true]);
            compare(a.next(), [8, false]);
            compare(a.next(), [6, false]);
            compare(a.next(), [4, false]);
            compare(a.next(), [2, false]);
            compare(a.next(), [10, true]);
            compare(a.next(), [8, false]);

            var a = new NextFacWrap(10, up, 1);
            compare(a.next(), [2, true]);
            compare(a.next(), [3, false]);

            var a = new NextFacWrap(2, dn, 1);
            compare(a.next(), [10, true]);
            compare(a.next(), [9, false]);

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

        function test_nextFocusNumber_div2() {
            var up = 1;

            compare(zl.nextFocusNumber(1, up, 2), 2);
            compare(zl.nextFocusNumber(8, up, 2), 10);
            compare(zl.nextFocusNumber(10, up, 2), 20);
            compare(zl.nextFocusNumber(20, up, 2), 40);
            compare(zl.nextFocusNumber(40, up, 2), 60);
            compare(zl.nextFocusNumber(60, up, 2), 80);
            compare(zl.nextFocusNumber(80, up, 2), 100);
            compare(zl.nextFocusNumber(100, up, 2), 200);
            compare(zl.nextFocusNumber(800, up, 2), 1000);
            compare(zl.nextFocusNumber(1000, up, 2), 2000);
        }

        function test_nextFocusNumber_div1() {
            var up = 1;

            compare(zl.nextFocusNumber(1, up, 1), 2);
            compare(zl.nextFocusNumber(2, up, 1), 3);
            compare(zl.nextFocusNumber(9, up, 1), 10);
            compare(zl.nextFocusNumber(10, up, 1), 20);
        }

        // These are from the spreadsheet used to prototype the
        // calculations.
        function xxtest_nextFocusNumber_spreadsheet() {
            // Division, Input, Expected Fwd, Expected Bak
            var rows = [
                [ 2, 1, 2, 0.8 ],
                [ 2, 2, 4, 1 ],
                [ 2, 4, 6, 2 ],
                [ 2, 6, 8, 4 ],
                [ 2, 8, 10, 6 ],
                [ 2, 10, 20, 8 ],
                [ 2, 20, 40, 10 ],
                [ 2, 40, 60, 20 ],
                [ 2, 60, 80, 40 ],
                [ 2, 80, 100, 60 ],
                [ 2, 100, 200, 80 ],
                [ 2, 200, 400, 100 ],
                [ 2, 400, 600, 200 ],
                [ 2, 600, 800, 400 ],
                [ 2, 800, 1000, 600 ],
                [ 2, 1000, 2000, 800 ],
                [ 2, 2000, 4000, 1000 ],
                [ 2, 4000, 6000, 2000 ],
                [ 5, 1, 5, 0.5 ],
                [ 5, 5, 10, 1 ],
                [ 5, 10, 50, 5 ],
                [ 5, 50, 100, 10 ],
                [ 5, 100, 500, 50 ],
                [ 5, 500, 1000, 100 ],
                [ 5, 1000, 5000, 500 ],
                [ 5, 5000, 10000, 1000 ],
                [ 5, 10000, 50000, 5000 ],
                [ 5, 50000, 100000, 10000 ],
                [ 1, 1, 2, 0.9 ],
                [ 1, 2, 3, 1 ],
                [ 1, 3, 4, 2 ],
                [ 1, 4, 5, 3 ],
                [ 1, 5, 6, 4 ],
                [ 1, 6, 7, 5 ],
                [ 1, 7, 8, 6 ],
                [ 1, 8, 9, 7 ],
                [ 1, 9, 10, 8 ],
                [ 1, 10, 20, 9 ],
                [ 1, 20, 30, 10 ],
                [ 1, 30, 40, 20 ],
                [ 1, 40, 50, 30 ],
                [ 1, 50, 60, 40 ],
                [ 1, 60, 70, 50 ],
                [ 1, 70, 80, 60 ],
                [ 1, 80, 90, 70 ],
                [ 1, 90, 100, 80 ],
                [ 1, 100, 200, 90 ],
                [ 1, 200, 300, 100 ],
                [ 1, 300, 400, 200 ],
            ];

            rows.forEach(function (row) {
                var division = row[0];
                var input = row[1];
                var fwd = row[2];
                var bkwd = row[3];
                var up = 1;
                var dn = -1;
                compare(zl.nextFocusNumber(input, up, division), fwd, row);
                compare(zl.nextFocusNumber(input, dn, division), bkwd, row);
            });
        }

    }
}
