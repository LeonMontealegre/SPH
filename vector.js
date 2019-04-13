
// Quick, easy constructor for a new Vector
function V(x = 0, y) {
    return new Vector(x, y);
}

// Constructor for a Vector object, containing an x and a y
function Vector(x, y) {
    if (x instanceof Vector) {
        this.x = x.x;
        this.y = x.y;
    } else if (y === undefined) {
        this.x = x;
        this.y = x;
    } else {
        this.x = x;
        this.y = y;
    }
}

Vector.prototype.add = function(v) {
    return V(this.x + v.x, this.y + v.y);
}

Vector.prototype.sub = function(v) {
    return V(this.x - v.x, this.y - v.y);
}

Vector.prototype.scl = function(v) {
    return V(this.x * v, this.y * v);
}

Vector.prototype.div = function(v) {
    return V(this.x / v, this.y / v);
}

Vector.prototype.dot = function(v) {
    return this.x * v.x + this.y * v.y;
}

Vector.prototype.cpy = function() {
    return V(this.x, this.y);
}

Vector.prototype.normalize = function() {
    var len = this.len();
    if (len === 0) {
        return V(0, 0);
    } else {
        return V(this.x / len, this.y / len);
    }
}

Vector.prototype.len = function() {
    return Math.sqrt(this.x*this.x + this.y*this.y);
}

Vector.prototype.len2 = function() {
    return (this.x*this.x + this.y*this.y);
}

Vector.prototype.distanceTo = function(vec) {
    return vec.sub(this).len();
}
