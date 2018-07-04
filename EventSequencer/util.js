// Taken from:
// https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Math/trunc
function trunc(v) {
    v = +v;
    return (v - v % 1)   ||   (!isFinite(v) || v === 0 ? v : v < 0 ? -0 : 0);
}

function negmod(a, b) {
    var r = a % b
    if (r > 0) r -= b
    return r
}

// Named after oracle SQL.
function nvl(expr1, expr2) {
    if (expr1 === undefined || expr1 === null) {
        return expr2;
    } else {
        return expr1;
    }
}
