function trunc(v) {
    if (v > 0) {
        return Math.floor(v)
    } else {
        return Math.ceil(v)
    }
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

function nvlobjprop(obj, prop, default_) {
    if (obj !== null && obj !== undefined) {
        return obj[prop]
    } else {
        return default_
    }
}

// Purpose: If "text" is bound to something, calling .mutable*() will wipe out
//          the text value before we get to save it.
function mutableobjset(mutablefn, prop, value) {
    mutablefn()[prop] = value
}
