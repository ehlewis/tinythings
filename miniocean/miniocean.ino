#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// MPU6050 gyroscope/accelerometer
Adafruit_MPU6050 mpu;

// MPU6050 orientation configuration
// Adjust these to match your physical mounting
#define INVERT_X false   // Set to true to flip X axis
#define INVERT_Y true    // Set to true to flip Y axis
#define SWAP_XY false     // Set to true to swap X and Y axes
#define ROTATION_DEG 90    // Rotate by 0, 90, 180, or 270 degrees

// Fluid simulation parameters
#define NUM_PARTICLES 80  // Adjust based on performance
#define PARTICLE_SIZE 2
#define DAMPING 0.98      // Velocity damping (energy loss)
#define GRAVITY_SCALE 0.3 // Scale for accelerometer input
#define MAX_VELOCITY 8.0  // Maximum particle velocity

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
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Initializing..."));
  display.display();
  
  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println(F("Failed to find MPU6050 chip"));
    display.println(F("MPU6050 failed!"));
    display.display();
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
  display.clearDisplay();
  drawParticles();
  display.display();
  
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
    float jitterStrength = 0.05;
    particles[i].vx += (random(-100, 100) / 100.0) * jitterStrength;
    particles[i].vy += (random(-100, 100) / 100.0) * jitterStrength;
    
    // Clamp velocity
    particles[i].vx = constrain(particles[i].vx, -MAX_VELOCITY, MAX_VELOCITY);
    particles[i].vy = constrain(particles[i].vy, -MAX_VELOCITY, MAX_VELOCITY);
    
    // Update position
    particles[i].x += particles[i].vx;
    particles[i].y += particles[i].vy;
    
    // Boundary collision with bounce
    if (particles[i].x <= PARTICLE_SIZE) {
      particles[i].x = PARTICLE_SIZE;
      particles[i].vx = -particles[i].vx * 0.7; // Bounce with energy loss
    }
    if (particles[i].x >= SCREEN_WIDTH - PARTICLE_SIZE) {
      particles[i].x = SCREEN_WIDTH - PARTICLE_SIZE;
      particles[i].vx = -particles[i].vx * 0.7;
    }
    if (particles[i].y <= PARTICLE_SIZE) {
      particles[i].y = PARTICLE_SIZE;
      particles[i].vy = -particles[i].vy * 0.7;
    }
    if (particles[i].y >= SCREEN_HEIGHT - PARTICLE_SIZE) {
      particles[i].y = SCREEN_HEIGHT - PARTICLE_SIZE;
      particles[i].vy = -particles[i].vy * 0.7;
    }
  }
  
  // Simple particle-particle interaction (cohesion/collision)
  handleParticleInteractions();
}

void handleParticleInteractions() {
  float interactionRadius = 6.0;
  float pushStrength = 0.8;  // Increased from 0.5
  float minDistance = 0.5;   // Minimum distance to prevent division by zero
  
  for (int i = 0; i < NUM_PARTICLES; i++) {
    for (int j = i + 1; j < NUM_PARTICLES; j++) {
      float dx = particles[j].x - particles[i].x;
      float dy = particles[j].y - particles[i].y;
      float distSq = dx * dx + dy * dy;
      float dist = sqrt(distSq);
      
      // If particles are too close, push them apart
      if (dist < interactionRadius) {
        // Prevent division by zero and handle overlapping particles
        if (dist < minDistance) {
          // Particles are nearly on top of each other - separate them forcefully
          float angle = random(0, 628) / 100.0; // Random angle in radians
          dx = cos(angle) * interactionRadius;
          dy = sin(angle) * interactionRadius;
          dist = interactionRadius;
          
          // Directly separate positions
          particles[i].x -= dx * 0.5;
          particles[i].y -= dy * 0.5;
          particles[j].x += dx * 0.5;
          particles[j].y += dy * 0.5;
        }
        
        // Calculate repulsion force
        float force = (interactionRadius - dist) / interactionRadius;
        float fx = (dx / dist) * force * pushStrength;
        float fy = (dy / dist) * force * pushStrength;
        
        // Apply force to velocities
        particles[i].vx -= fx;
        particles[i].vy -= fy;
        particles[j].vx += fx;
        particles[j].vy += fy;
        
        // Additional positional correction to prevent overlap
        float overlap = (interactionRadius - dist) * 0.5;
        if (overlap > 0) {
          float separationX = (dx / dist) * overlap;
          float separationY = (dy / dist) * overlap;
          
          particles[i].x -= separationX;
          particles[i].y -= separationY;
          particles[j].x += separationX;
          particles[j].y += separationY;
        }
      }
    }
  }
}

void drawParticles() {
  // Draw particles with motion blur for smoother appearance
  for (int i = 0; i < NUM_PARTICLES; i++) {
    // Draw particle
    display.fillCircle((int)particles[i].x, (int)particles[i].y, PARTICLE_SIZE, SSD1306_WHITE);
    
    // Optional: Draw motion trail
    float speed = sqrt(particles[i].vx * particles[i].vx + particles[i].vy * particles[i].vy);
    if (speed > 1.0) {
      display.drawLine(
        (int)particles[i].px, (int)particles[i].py,
        (int)particles[i].x, (int)particles[i].y,
        SSD1306_WHITE
      );
    }
  }
  
  // Optional: Display info
  // display.setCursor(0, 0);
  // display.print(F("X:"));
  // display.print(accelX, 1);
  // display.print(F(" Y:"));
  // display.print(accelY, 1);
}
