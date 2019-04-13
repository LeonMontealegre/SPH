class Particle {
    constructor(pos, mass, restDensity, viscosity, gasConstant) {
        this.pos = pos;
        this.vel = V();
        this.acc = V();

        this.mass = mass ? mass : 0.02;
        this.restDensity = restDensity ? restDensity : 998.29;
        this.viscosity = viscosity ? viscosity : 3.5;
        this.gasConstant = gasConstant ? gasConstant : 3;

        this.density = 0;
        this.pressure = 0;
        // this.neighbors = [];
    }
    // step() {
    //     // Simple Euler integration
    //     this.vel = this.vel.add(this.acc.scl(dt));
    //     this.pos = this.pos.add(this.vel.scl(dt));
    //
    //     this.acc = V();
    //
    //     const amt = -0.9
    //     if (this.pos.x < box.l) {
    //         this.vel.x *= amt;
    //         this.pos.x = box.l;
    //     }
    //     if (this.pos.x > box.r) {
    //         this.vel.x *= amt;
    //         this.pos.x = box.r;
    //     }
    //     if (this.pos.y < box.b) {
    //         this.vel.y *= amt;
    //         this.pos.y = box.b;
    //     }
    //     if (this.pos.y > box.t) {
    //         this.vel.y *= amt;
    //         this.pos.y = box.t;
    //     }
    //     // var v = this.vel.len();
    //     // this.pos = this.pos.add(this.vel.scl(dt));
    //     // // this.vel = this.vel.scl(0.99);
    //     //
    //     // this.addForce(V(0, g * this.mass));
    //     // if (this.pos.y < 0) {
    //     //     this.vel.y *= -1;
    //     //     this.pos.y = 0;
    //     // }
    //     //     // this.addForce(V(0, 100*(0 - this.pos.y)));
    //     //
    //     return 0.5 * this.mass * Math.pow(this.vel.len(), 2);
    // }
    accelerate(a) {
        // Newton's law a = F/m
        this.acc = a;
    }
    draw(ctx) {
        ctx.fillStyle = this.restDensity >= 998 ? '#ff00ff' : '#ff0000';
        if (selectedParticle == this)
            ctx.fillStyle = '#00ff00';
        ctx.beginPath();
        ctx.arc(this.pos.x, -this.pos.y, (box.r - box.l)/100, 0, 2*Math.PI);
        ctx.fill();
        ctx.closePath();
    }
}
