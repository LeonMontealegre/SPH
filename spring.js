class Spring {
    constructor(p1, p2, k, len, min, max) {
        this.p1 = p1;
        this.p2 = p2;
        this.len = (len === undefined ? p1.pos.sub(p2.pos).len() : len);
        this.min = min;
        this.max = max;
        this.k = k;
    }
    step() {
        var p1 = this.p1;
        var p2 = this.p2;
        
        var curLen = p1.pos.sub(p2.pos).len();
        
        var x = curLen - this.len;

        var dir1 = p1.pos.sub(p2.pos).normalize();
        p1.addForce(dir1.scl(-this.k * (curLen - this.len)/2));
        var dir2 = dir1.scl(-1);
        p2.addForce(dir2.scl(-this.k * (curLen - this.len)/2));
        
        return 0.5 * this.k * Math.pow(x, 2);
    }
    draw(ctx) {
        ctx.strokeStyle = '#000000';
        ctx.beginPath();
        ctx.moveTo(this.p1.pos.x, -this.p1.pos.y);
        ctx.lineTo(this.p2.pos.x, -this.p2.pos.y);
        ctx.stroke();
        ctx.closePath();
    }
}
