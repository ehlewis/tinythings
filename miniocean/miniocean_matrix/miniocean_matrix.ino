#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// MAX7219 LED Matrix settings
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW  // Change based on your hardware type
// Other options: PAROLA_HW, GENERIC_HW, ICSTATION_HW, DR0CR0RR0_HW
#define MAX_DEVICES 4      // Number of 8x8 matrices (4 = 32x8 display)
#define CS_PIN 5           // Chip Select pin
#define SCREEN_WIDTH (MAX_DEVICES * 2)  // Total width
#define SCREEN_HEIGHT 8    // Height is always 8 for MAX7219

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// MPU6050 gyroscope/accelerometer
Adafruit_MPU6050 mpu;

// MPU6050 orientation configuration
// Adjust these to match your physical mounting
#define INVERT_X true    // Set to true to flip X axis
#define INVERT_Y true    // Set to true to flip Y axis
#define SWAP_XY false     // Set to true to swap X and Y axes
#define ROTATION_DEG 0    // Rotate by 0, 90, 180, or 270 degrees

// Fluid simulation parameters
#define NUM_PARTICLES 25  // Slightly increased for better fluid appearance
#define PARTICLE_SIZE 1   // Single pixel for LED matrix
#define DAMPING 0.92      // Increased damping (more energy loss = less bouncing)
#define GRAVITY_SCALE 0.5 // Increased gravity for more responsive movement
#define MAX_VELOCITY 2.0  // Much lower max velocity for smoother, fluid-like motion

// Particle structure
struct Particle {
  float x, y;      // Position
  float vx, vy;    // Velocity
  float px, py;    // Previous position (for collision)
};

Particle particles[NUM_PARTICLES];

// Accelerometer data
float accelX = 0, accelY = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C for MPU6050
  Wire.begin();
  
  // Initialize MAX7219
  mx.begin();
  mx.clear();
  mx.control(MD_MAX72XX::INTENSITY, 2);  // Set brightness (0-15)
  
  // Display initialization message
  Serial.println(F("Initializing..."));
  
  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println(F("Failed to find MPU6050 chip"));
    for(;;);
  }
  Serial.println(F("MPU6050 Found!"));
  
  // Set accelerometer range (±2g, ±4g, ±8g, or ±16g)
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  
  // Set gyro range (not used in this sketch but good to set)
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  
  // Set filter bandwidth
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  // Calibrate MPU6050 (keep board flat and still)
  delay(1000);
  
  // Initialize particles randomly
  for (int i = 0; i < NUM_PARTICLES; i++) {
    particles[i].x = random(PARTICLE_SIZE, SCREEN_WIDTH - PARTICLE_SIZE);
    particles[i].y = random(PARTICLE_SIZE, SCREEN_HEIGHT - PARTICLE_SIZE);
    particles[i].vx = 0;
    particles[i].vy = 0;
    particles[i].px = particles[i].x;
    particles[i].py = particles[i].y;
  }
  
  delay(500);
}

void rotateAccelerometer(float &x, float &y) {
  float tempX = x;
  float tempY = y;
  
  // Apply rotation
  switch(ROTATION_DEG) {
    case 90:
      x = -tempY;
      y = tempX;
      break;
    case 180:
      x = -tempX;
      y = -tempY;
      break;
    case 270:
      x = tempY;
      y = -tempX;
      break;
    default: // 0 degrees
      // No rotation needed
      break;
  }
  
  // Swap axes if needed
  if (SWAP_XY) {
    tempX = x;
    x = y;
    y = tempX;
  }
  
  // Invert axes if needed
  if (INVERT_X) x = -x;
  if (INVERT_Y) y = -y;
}

void loop() {
  // Read accelerometer data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  
  // Convert to screen coordinates
  // a.acceleration.x/y/z is in m/s^2
  // Normalize by dividing by 9.8 (1g) and apply scaling
  float rawX = (a.acceleration.x / 9.8);
  float rawY = (a.acceleration.y / 9.8);
  
  // Apply rotation and axis configuration
  rotateAccelerometer(rawX, rawY);
  
  // Scale to screen coordinates
  accelX = rawX * GRAVITY_SCALE * SCREEN_WIDTH;
  accelY = rawY * GRAVITY_SCALE * SCREEN_HEIGHT;
  
  // Update particles
  updateParticles();
  
  // Render
  mx.clear();
  drawParticles();
  
  // Small delay for stability (~30-60 fps)
  delay(16);
}

void updateParticles() {
  for (int i = 0; i < NUM_PARTICLES; i++) {
    // Store previous position
    particles[i].px = particles[i].x;
    particles[i].py = particles[i].y;
    
    // Apply gravity/acceleration from gyroscope
    particles[i].vx += accelX * 0.1;
    particles[i].vy += accelY * 0.1;
    
    // Apply damping
    particles[i].vx *= DAMPING;
    particles[i].vy *= DAMPING;
    
    // Add small random "jitter" to prevent particles from getting stuck
    // This helps unstick collapsed particles
    float jitterStrength = 0.02;  // Reduced for smoother motion
    particles[i].vx += (random(-100, 100) / 100.0) * jitterStrength;
    particles[i].vy += (random(-100, 100) / 100.0) * jitterStrength;
    
    // Clamp velocity
    particles[i].vx = constrain(particles[i].vx, -MAX_VELOCITY, MAX_VELOCITY);
    particles[i].vy = constrain(particles[i].vy, -MAX_VELOCITY, MAX_VELOCITY);
    
    // Update position
    particles[i].x += particles[i].vx;
    particles[i].y += particles[i].vy;
    
    // Boundary collision with minimal bounce (more sticky)
    if (particles[i].x <= PARTICLE_SIZE) {
      particles[i].x = PARTICLE_SIZE;
      particles[i].vx = -particles[i].vx * 0.2; // Much less bounce
    }
    if (particles[i].x >= SCREEN_WIDTH - PARTICLE_SIZE) {
      particles[i].x = SCREEN_WIDTH - PARTICLE_SIZE;
      particles[i].vx = -particles[i].vx * 0.2;
    }
    if (particles[i].y <= PARTICLE_SIZE) {
      particles[i].y = PARTICLE_SIZE;
      particles[i].vy = -particles[i].vy * 0.2;
    }
    if (particles[i].y >= SCREEN_HEIGHT - PARTICLE_SIZE) {
      particles[i].y = SCREEN_HEIGHT - PARTICLE_SIZE;
      particles[i].vy = -particles[i].vy * 0.2;
    }
  }
  
  // Simple particle-particle interaction (cohesion/collision)
  handleParticleInteractions();
}

void handleParticleInteractions() {
  float cohesionRadius = 5.0;     // Distance for attraction
  float separationRadius = 1.5;   // Very close distance for slight separation
  float cohesionStrength = 0.15;  // Pull particles together
  float separationStrength = 0.3; // Gentle push when too close
  float minDistance = 0.5;
  
  for (int i = 0; i < NUM_PARTICLES; i++) {
    for (int j = i + 1; j < NUM_PARTICLES; j++) {
      float dx = particles[j].x - particles[i].x;
      float dy = particles[j].y - particles[i].y;
      float dist = sqrt(dx * dx + dy * dy);
      
      if (dist < minDistance) {
        // Particles too close - minimal separation
        float angle = random(0, 628) / 100.0;
        dx = cos(angle) * separationRadius;
        dy = sin(angle) * separationRadius;
        dist = separationRadius;
        
        particles[i].x -= dx * 0.3;
        particles[i].y -= dy * 0.3;
        particles[j].x += dx * 0.3;
        particles[j].y += dy * 0.3;
      }
      else if (dist < separationRadius) {
        // Very close - gentle push apart
        float force = (separationRadius - dist) / separationRadius;
        float fx = (dx / dist) * force * separationStrength;
        float fy = (dy / dist) * force * separationStrength;
        
        particles[i].vx -= fx;
        particles[i].vy -= fy;
        particles[j].vx += fx;
        particles[j].vy += fy;
      }
      else if (dist < cohesionRadius) {
        // Medium range - pull together (cohesion for fluid effect)
        float force = (dist / cohesionRadius) * cohesionStrength;
        float fx = (dx / dist) * force;
        float fy = (dy / dist) * force;
        
        particles[i].vx += fx;
        particles[i].vy += fy;
        particles[j].vx -= fx;
        particles[j].vy -= fy;
      }
    }
  }
}

void drawParticles() {
  // Draw particles on LED matrix
  for (int i = 0; i < NUM_PARTICLES; i++) {
    int px = (int)particles[i].x;
    int py = (int)particles[i].y;
    
    // Ensure coordinates are within bounds
    if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
      mx.setPoint(py, px, true);  // setPoint(row, col, state)
    }
  }
}
