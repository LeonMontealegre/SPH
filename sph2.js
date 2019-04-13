// Mouse position
var mx = -1, my = -1;

const g = V(0, -9.81);

const ms = 16;
let dt = 1.0 / 100.0;

const GAS_STIFFNESS = 3;//k = 3;//8.314;
const REST_DENSITY = 998.29;//ρ0 = 998.29;//100;

const SURFACE_TENSION = 0.0728;//σ = 0.0728;
const SURFACE_THRESHOLD = 7.065;//surfaceThreshold = 7.065;

const h = 0.0457;//0.4;
const h2 = h*h;

const VISCOSITY = 3.5;//μ = 3.5;//0.8;

const WALL_K = 10000.0;
const WALL_DAMPING = -0.9;

let selectedParticle = undefined;

const box = {l: 0, r: 0.5, b: 0, t: 0.5};

class Wall {
    constructor(normal, pos) {
        this.pos = pos;
        this.normal = normal;
    }
}

let walls = [new Wall(V(1, 0), V(box.l, 0)), new Wall(V(-1,  0), V(box.r, 0)),
             new Wall(V(0, 1), V(0, box.b)), new Wall(V( 0, -1), V(0, box.t))];

var time = 0;

var particles = [];


// Load canvas to draw stuff
const canvas = document.getElementById("canvas");
const ctx = canvas.getContext('2d');

canvas.addEventListener('click', onMouseClick, false);
canvas.addEventListener('mousemove', onMouseMove, false);
window.addEventListener('keyup', onKeyUp, false);
window.addEventListener('resize', resize, false);
resize();

const tempText = document.getElementById("temp");
const matText = document.getElementById("mat");
const timeSlider = document.getElementById("timeSlider");

function start() {
    time = 0;
    particles = [];
    // const dx = (box.r - box.l) / 25;
    // const dy = (box.t - box.b) / 25;
    // for (let x = box.l; x < box.r; x += dx) {
    //     for (let y = box.b; y < box.t; y += dy) {
    //         let rd = REST_DENSITY;
    //         if (Math.random() > 0.5)
    //             rd = 500;
    //         particles.push(new Particle(V(x+(box.r - box.l)/100,
    //                                       y+(box.r - box.l)/100), 0.02, rd));
    //     }
    // }

    const dx = (box.r - box.l) / 25;
    const dy = (box.t - box.b) / 25;
    for (let x = box.l; x < box.r; x += dx) {
        for (let y = box.b; y < box.t; y += dy) {
            particles.push(new Particle(V(x+(box.r - box.l)/100,
                                          y+(box.r - box.l)/100), 0.02));
        }
    }

    // particles.push(new Particle(V(box.l, box.b), 0.02));
    // particles.push(new Particle(V(box.r, box.t), 0.02));
    // particles.push(new Particle(V(50,50), 1));

    draw();
}

function Wpoly6(r2) {
    // Poly 6 W
    const coef = 315.0 / (64.0 * Math.PI * Math.pow(h, 9));

    if (0 <= r2 && r2 <= h2)
        return coef * Math.pow(h2 - r2, 3);
    else
        return 0;
}

function Wpoly6Gradient(r, r2) {
    const coef = -945.0 / (32.0 * Math.PI * Math.pow(h, 9));

    if (0 <= r2 && r2 <= h2)
        return r.scl(coef * Math.pow(h2 - r2, 2));
    else
        return V(0,0);
}

function Wpoly6Laplacian(r2) {
    const coef = -45.0 / (Math.PI * Math.pow(h, 6));

    if (0 <= r2 && r2 <= h2)
        return coef * (h2 - r2) * (3.0 * h2 - 7.0 * r2);
    else
        return 0;
}

function WspikyGradient(r, r2) {
    const coef = -45.0 / (Math.PI * Math.pow(h, 6));
    const radius = Math.sqrt(r2);

    if (0 <= r2 && r2 <= h2)
        return r.scl(coef * Math.pow(h - radius, 2) / radius);
    else
        return V(0,0);
}

function WviscosityLaplacian(r2) {
    const coef = 45.0 / (Math.PI * Math.pow(h, 6));
    const radius = Math.sqrt(r2);

    if (0 <= r2 && r2 <= h2)
        return coef * (h - radius);
    else
        return 0;
}

function collisionForce(particle) {
    for (let i = 0; i < walls.length; i++) {
        const wall = walls[i];

        const d = (wall.pos.sub(particle.pos)).dot(wall.normal);// + 0.01;

        if (d > 0.0) {
            particle.acc = particle.acc.add(wall.normal.scl(WALL_K * d));
            particle.acc = particle.acc.add(wall.normal.scl(WALL_DAMPING * particle.vel.dot(wall.normal)));
        }
    }
}

function step() {
    // particles.push(new Particle(V(box.r - 0.05,
    //                               box.t), 0.02, REST_DENSITY/2));
    // particles[particles.length-1].vel = V(-1, 0);
    // particles.push(new Particle(V((box.r - box.l)/2 + h/2,
    //                               box.t), 0.02, REST_DENSITY/2));
    // particles.push(new Particle(V((box.r - box.l)/2 - h/2,
    //                               box.t), 0.02, REST_DENSITY/2));
    // console.log(particles.length);

    // Calculate density and pressure
    for (let i = 0; i < particles.length; i++) {
        const particle = particles[i];

        // particle.neighbors = [];
        particle.density = 0.0;

        // Find neighbors
        for (let j = 0; j < particles.length; j++) {
            if (i == j)
                continue;

            const neighbor = particles[j];

            const r = particle.pos.sub(neighbor.pos);
            const r2 = r.len2();

            if (r2 <= h*h) {
                const density = neighbor.mass * Wpoly6(r2);
                particle.density += density;
                // particle.neighbors.push(neighborParticle);
            }
        }

        particle.pressure = particle.gasConstant * (particle.density - particle.restDensity);
    }

    // Calculate forces
    for (let i = 0; i < particles.length; i++) {
        const particle = particles[i];

        let f_pressure = V(0, 0);
        let f_viscosity = V(0, 0);
        let f_surface = V(0, 0);

        let f_gravity = g.scl(particle.density);

        let colorFieldNormal = V(0, 0);
        let colorFieldLaplacian = 0;

        let neighborCount = 0;

        for (let j = 0; j < particles.length; j++) {//particle.neighbors.length; j++) {
            const neighbor = particles[j];//.neighbors[j];

            const r = particle.pos.sub(neighbor.pos);
            const r2 = r.len2();

            if (r2 <= h*h) {
                neighborCount++;
                const poly6Gradient = Wpoly6Gradient(r, r2);
                const spikyGradient = WspikyGradient(r, r2);

                if (i != j) {
                    f_pressure = f_pressure.add(spikyGradient.scl(-neighbor.mass * (particle.pressure + neighbor.pressure) / (2 * neighbor.density)));

                    f_viscosity = f_viscosity.add((neighbor.vel.sub(particle.vel)).scl(
                                    (particle.viscosity + neighbor.viscosity) / 2 * neighbor.mass *
                                    WviscosityLaplacian(r2) / neighbor.density));
                }

                colorFieldNormal = colorFieldNormal.add(poly6Gradient.scl(neighbor.mass / neighbor.density));

                colorFieldLaplacian += neighbor.mass * Wpoly6Laplacian(r2) / neighbor.density;
            }
        }

        const colorFieldNormalMagnitude = colorFieldNormal.len();
        if (colorFieldNormalMagnitude > SURFACE_THRESHOLD) {
            f_surface = colorFieldNormal.scl(-SURFACE_TENSION * colorFieldLaplacian / colorFieldNormalMagnitude);
        }

        const force = f_pressure.add(f_viscosity).add(f_surface).add(f_gravity);

        particle.accelerate(force.scl(1.0 / particle.density));

        collisionForce(particle);
    }

    // Step using Verlet Integration
    for (let i = 0; i < particles.length; i++) {
        const particle = particles[i];

        const acc = particle.acc;

        // particle.vel = particle.vel.add(acc.scl(dt));
        // particle.pos = particle.pos.add(particle.vel.scl(dt));

        const newPos = particle.pos.add(particle.vel.scl(dt)).add(particle.acc.scl(dt*dt));
        const newVel = (newPos.sub(particle.pos)).scl(1.0 / dt);

        particle.pos = newPos;
        particle.vel = newVel;
    }
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

function timeChange() {
    dt = timeSlider.value;
    console.log(dt);
}

var interval = undefined;
function pause() {
    console.log("pause");

    clearInterval(interval);
    interval = undefined;
}
function play() {
    console.log("play");

    interval = setInterval(function() {
        time += dt;
        step();
        draw();
    }, ms);
}

function onKeyUp(e) {
    if (e.keyCode == 65) {
        if (interval)
            pause();
        else
            play();
    }

    if (e.keyCode == 32) {
        console.log("step");

        step();
        draw();
    }
}

function onMouseClick(e) {
    var pos = V(mx, my);
    console.log(pos);
    // Click nearest particle
    let min = {len: Infinity, particle: undefined}
    for (let i = 0; i < particles.length; i++) {
        const particle = particles[i];

        // Find min length
        const len = particle.pos.sub(pos).len();
        if (len < min.len) {
            min.len = len;
            min.particle = particle;
        }
    }

    if (min.particle != undefined) {
        selectedParticle = min.particle;
        draw();
    }
    // step();
    // draw();
}

function onMouseMove(e) {
    var rect = canvas.getBoundingClientRect();
    var x = (e.clientX - rect.left) / rect.width  * (box.r - box.l) + box.l;
    var y = (e.clientY - rect.top)  / rect.height * (box.t - box.b) + box.b;
    mx = x;
    my = box.t - y;
}

function resize(e) {
    canvas.width = window.innerWidth*0.96;
    canvas.height = (window.innerHeight - document.getElementById("menutop").clientHeight)*0.96;

    draw();
}
