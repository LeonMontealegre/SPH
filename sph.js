// Load canvas to draw stuff
var canvas = document.getElementById("canvas");
canvas.width = window.innerWidth;
canvas.height = window.innerHeight;
document.body.insertBefore(canvas, document.body.childNodes[0]);
var ctx = canvas.getContext('2d');
canvas.addEventListener('click', onMouseClick, false);
canvas.addEventListener('mousemove', onMouseMove, false);
window.addEventListener('keyup', onKeyUp, false);
window.addEventListener('resize', resize, false);
var tempText = document.getElementById("temp");
var matText = document.getElementById("mat");

// Mouse position
var mx = -1, my = -1;

const g = V(0, -9.81);

const ms = 16;
const dt = 0.001;

const GAS_CONSTANT = 3;//8.314;
const REST_DENSITY = 998.29;//100;

const SURFACE_TENSION = 0.0728;
const SURFACE_TENSION_THRESHOLD = 7.065;

const h = 0.0457
const h2 = h*h;;

const VISCOSITY = 3.5;//μ = 3.5;//0.8;

const box = {l: 0, r: 0.4, b: 0, t: 0.4};

var time = 0;

var particles = [];
var springs = [];

function start() {
    // var p1 = new Particle(V(0, 0), 1);
    // particles.push(p1);
    // var p2 = new Particle(V(50,0), 1);
    // particles.push(p2);
    // var p3 = new Particle(V(25,-30),1);
    // particles.push(p3);
    // var s1 = new Spring(p1, p2);
    // springs.push(s1);
    // var s2 = new Spring(p1, p3);
    // springs.push(s2);
    // var s3 = new Spring(p2, p3);
    // springs.push(s3);

    // for (var a = -25; a < 25; a += 50.0/12.5)
    //     particles.push(new Particle(V(a, -25), 1));
    // for (var a = -25; a < 25; a += 50.0/12.5)
    //     particles.push(new Particle(V(25, a), 1));
    // for (var a = 25; a > -25; a -= 50.0/12.5)
    //     particles.push(new Particle(V(a, 25), 1));
    // for (var a = 25; a > -25; a -= 50.0/12.5)
    //     particles.push(new Particle(V(-25, a), 1));


    // for (var a = 0; a < 2*Math.PI; a += 2*Math.PI/3) {
    //     var x = 50*Math.cos(a);
    //     var y = 50*Math.sin(a) + 100;
    //     particles.push(new Particle(V(x, y), 1));
    // }
    // var r = new Particle(V(0, 100), 1);
    //
    // for (var i = 0; i < particles.length; i++) {
    //     var spring1 = new Spring(particles[i], particles[(i+1)%particles.length], 40);
    //     springs.push(spring1);
    //     var spring2 = new Spring(particles[i], r, 40);
    //     springs.push(spring2);
    // }
    //
    // particles.push(r);


    const dx = (box.r - box.l) / 20;
    const dy = (box.t - box.b) / 20;
    for (let x = box.l; x < box.r; x += dx) {
        for (let y = box.b; y < box.t; y += dy) {
            particles.push(new Particle(V(x+(box.r - box.l)/100,y+(box.r - box.l)/100), 0.02));
        }
    }

    // particles.push(new Particle(V(0.5,0.5), 1));
    // particles.push(new Particle(V(50,50), 1));
    //
    // particles[0].addForce(V(0, 50));
    // particles[1].addForce(V(0, 50));

    draw();

    // setInterval(function() {
    //     time += dt;
    //     step();
    //     draw();
    // }, ms);
}

function W(i, j) {
    // Poly 6 W
    const r = particles[j].pos.sub(particles[i].pos);
    const rr = r.len();

    if (0 <= rr && rr <= h)
        return 315 / (64 * Math.PI * Math.pow(h, 9)) * Math.pow((h*h - rr*rr), 3);
    else
        return 0;
}

function dW0(i, j) {
    // Poly 6 W
    const r = particles[j].pos.sub(particles[i].pos);
    const rr = r.len();

    if (0 <= rr && rr <= h)
        return r.scl(-945 / (320 * Math.PI * Math.pow(h, 9)) * Math.pow((h*h - rr*rr), 2));
    else
        return V(0,0);
}

function ddW0(i, j) {
    // Poly 6 W
    const r = particles[j].pos.sub(particles[i].pos);
    const rr = r.len();

    if (0 <= rr && rr <= h)
        return -945 / (32 * Math.PI * Math.pow(h, 9)) * (3*h*h - 7*rr*rr);
    else
        return 0;
}

function dW(i, j) {
    // Spiky W
    const r = particles[i].pos.sub(particles[j].pos);
    const rr = r.len();

    if (0 <= rr && rr <= h)
        return r.scl(-45 / (Math.PI * Math.pow(h, 6)) * Math.pow((h - rr), 2)).scl(1 / rr);
    else
        return V(0,0);
}

function ddW(i, j) {
    // Viscosity W
    const r = particles[j].pos.sub(particles[i].pos);
    const rr = r.len();

    if (0 <= rr && rr <= h)
        return 45 / (Math.PI * Math.pow(h, 6)) * (h - rr);//15 / (2 * Math.PI * Math.pow(h, 3)) * (-3*rr / Math.pow(h, 3) + 2 / Math.pow(h, 2) + h / Math.pow(rr, 3));
    else
        return 0;
}

function collectNeighbors(i) {
    let neighbors = [];
    for (let j = 0; j < particles.length; j++) {
        if (i == j)
            continue;

        if (particles[i].pos.distanceTo(particles[j].pos) < h)
            neighbors.push(j);
    }
    return neighbors;
}

function calculateDensity(i, neighbors) {
    let ρ = 0;
    for (let j = 0; j < neighbors.length; j++) {
        ρ += particles[neighbors[j]].mass * W(i, neighbors[j]);
    }
    return ρ == 0 ? 1 : ρ;
}

function calculatePressure(i, neighbors, densities) {
    return k*(densities[i] - ρ0);
}

function calculateForce(i, neighbors, densities, pressures) {
    // console.log(neighbors);
    cfN = V(0, 0);
    cFL = 0;
    fs = V(0, 0);
    // Calculate surface tension force
    for (let j = 0; j < neighbors.length; j++) {
        let n = neighbors[j];
        cfN = cfN.add(dW0(i, n).scl(particles[n].mass / densities[n]));
        cFL += particles[n].mass * ddW0(i, n) / densities[n];
    }

    if (cfN.len() > surfaceThreshold) {
        fs = cfN.normalize().scl(-σ * cFL);
    }

    // Pressure force
    let fp = V(0,0);
    for (let j = 0; j < neighbors.length; j++) {
        let n = neighbors[j];
        fp = fp.add(dW(i, n).scl(pressures[i]/Math.pow(densities[i], 2) + pressures[n]/Math.pow(densities[n], 2)).scl(-particles[n].mass / densities[i]))
        // fp = fp.add(dW(i, n).scl(-particles[n].mass / densities[n] * (pressures[i] + pressures[n]) / 2));
    }
    // console.log(fp);

    // Viscosity force
    let fv = V(0,0);
    for (let j = 0; j < neighbors.length; j++) {
        let n = neighbors[j];
        fv = fv.add((particles[n].vel.sub(particles[i].vel).scl(particles[n].mass * ddW(i, n) / densities[n])));
        // fv = fv.add(particles[n].vel.sub(particles[i].vel).scl(particles[n].mass / densities[j]).scl(ddW(i, n))));
    }
    fv = fv.scl(μ);

    // Gravity force
    let fg = g.scl(densities[i]);

    return fp.add(fv).add(fg).add(fs);
}

function step() {
    // Find neighbors
    let N = [];
    for (let i = 0; i < particles.length; i++) {
        let n = collectNeighbors(i);
        N.push(n);
    }

    // Compute density + pressure
    let ρ = [];
    let p = [];
    for (let i = 0; i < particles.length; i++) {
        ρ.push(calculateDensity(i, N[i]));
        p.push(calculatePressure(i, N[i], ρ));
    }

    // Compute forces
    for (let i = 0; i < particles.length; i++) {
        let force = calculateForce(i, N[i], ρ, p);
        particles[i].accelerate(force.scl(1 / ρ[i]));
    }

    // Move the particles
    for (let i = 0; i < particles.length; i++) {
        particles[i].step();
    }


    // var particleKEnergy = 0;
    //
    // for (var i = 0; i < particles.length; i++)
    //     particleKEnergy += particles[i].step();
    //
    // var totalEnergy = particleKEnergy;
    // console.log("Total energy in the system: " + particleKEnergy + " + " + 0 + " = " + totalEnergy);
}

function draw() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.save();
    ctx.scale(canvas.width / (box.r - box.l), canvas.height / (box.t - box.b));
    ctx.translate(0, box.t);

    for (var i = 0; i < particles.length; i++)
        particles[i].draw(ctx);

    // ctx.strokeStyle = '#000000';
    // ctx.beginPath();
    // ctx.moveTo(-canvas.width/2, 0);
    // ctx.lineTo(canvas.width/2, 0);
    // ctx.stroke();
    // ctx.closePath();

    ctx.restore();
}

function onKeyUp(e) {
    if (e.keyCode == 65) {
        console.log("start");
        setInterval(function() {
            time += dt;
            step();
            draw();
        }, ms);

    }
    if (e.keyCode == 32) {
        console.log("step");
        step();
        draw();
    }
}

function onMouseClick(e) {
    // step();
    draw();
}

function onMouseMove(e) {
    var x = e.clientX;//Math.floor((e.clientX - 9) / cw);
    var y = e.clientY;//Math.floor((e.clientY - 9) / ch);
    mx = x;
    my = y;
}

function resize(e) {
    canvas.width = window.innerWidth*0.96;
    canvas.height = window.innerHeight*0.96;

    draw();
}
