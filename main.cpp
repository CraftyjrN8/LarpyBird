#include <raylib.h>
#include <random>
#include <raymath.h>
#include <iostream>
#include <cmath>

float roundTo(float value, int places) {
    float factor = powf(10.0f, (float)places);
    return roundf(value * factor) / factor;
}

class Ground {
  public:
    float leftx = -103.0f;
    Vector2 pos = { 0 };
    Texture2D groundTex;
    float speed = 120.0f;

    Ground(Texture2D tex, float x, float y){
      this->groundTex = tex;
      this->pos.x = x;
      this->pos.y = y;
    }

    void updateGround(float& dt){
      pos.x -= speed * dt;

      if (pos.x < leftx){
        pos.x = 0;
      }

      DrawTextureV(groundTex, pos, WHITE);
    }
};

class Player {
  public:
    Vector2 pos;
    Vector2 vel = {0};
    Texture2D birdTex;

    Sound deathSound;
    Sound jumpSound;

    const float imgScale = 0.035f;
    float jumpStrength = 450.0f;
    float rotation = 0.0f;

    float width;
    float height;

    bool dead = false;
    float deathElapsed = 0.0f;
    bool deb1 = false;
    bool deb2 = false;
    bool deb3 = false;


    Rectangle source;
    Rectangle dest;
    Vector2 origin;

    float hitW = 45;
    float hitH = 30;

    Rectangle hitbox;
    Player(float x, float y, Texture2D texture, Sound death, Sound jumpS){
      this->birdTex = texture;
      this->pos.x = x;
      this->pos.y = y;
      this->deathSound = death;
      this->jumpSound = jumpS;

      width = birdTex.width * imgScale;
      height = birdTex.height * imgScale;

      source = {0, 0, (float)birdTex.width, (float)birdTex.height};
      dest = {this->pos.x,this->pos.y, this->width, this->height};
      origin = {this->width / 2, this->height / 2};

    }

    void updatePlayer(float& gravity, float& dt, bool& start){

      if (this->pos.y < 15){
        this->vel.y = 100;
      } 
      if (this->vel.y < 1000) {
        this->vel.y += (gravity * dt);
      }

      if (this->pos.y >= (400-(this->hitH/2))) {
        this->pos.y = 400-(this->hitH/2);
        this->vel.y = 0;
      }

      if (IsKeyPressed(KEY_UP)){
        jump();
        if (!start) {
          start = true;
        }
      } 
      if (dead) {
        

        if (roundf(pos.y) == 385 && deb3 == false && deb2 == true){
          deb3 =true;
          this->vel.y = -100;
        }

        if (roundf(pos.y) == 385 && deb2 == false && deb1 == true){
          deb2 = true;
          this->vel.y = -300;
        }

        if (roundf(pos.y) == 385 && deb1 == false){
          deb1 = true;
          this->vel.y = -600;
        }


      }
    
      move(dt);
      rotation = Clamp((vel.y / 20.0f), -60.0f, 60.0f);
      dest = {this->pos.x,this->pos.y, this->width, this->height};
      this->hitbox = {pos.x - hitW / 2, pos.y - hitH/2, hitW, hitH};

      
      DrawTexturePro(this->birdTex, this->source, this->dest, this->origin, this->rotation, WHITE);

    }
    void jump(){
      if (!this->dead){
        this->vel.y = -this->jumpStrength;
        PlaySound(jumpSound);
      }
    }

    void move(float& dt){
       this->pos.y += this->vel.y * dt;
    }

    Rectangle getHitbox() const{
      return {this->hitbox};
    }

    void death(float& dt){
      if (dead) return;
      PlaySound(deathSound);
      dead = true;
      deathElapsed = 0.0f;
      vel.y = -600;
    }

    bool getDeath(){
      if (dead){
        return true;
      } else{
        return false;
      }
    }

    void reset(){
      pos = {200, 225};
      vel = {0,0};
      rotation = 0.0f;
      dead = false;
      deathElapsed = 0.0f;
      deb1 = false;
      deb2 = false;
      deb3 = false;

    }

};

class Pipes {
    
  public:
    Texture2D pipeTex;
    Vector2 pos;
    float pipespeed = 200.0f;
    Vector2 size = {40,500};
    bool scoreDebounce = false;
    float gap = 102.0f;
    std::mt19937 gen{std::random_device{}()};
    std::uniform_int_distribution<int> distr{240,450};
    
    float imgScale = 0.12;
    float width;
    float height;

    float ogX;

    Vector2 origin;

    float hitW = 75.0f;
    float hitH = 383.0f;

    Rectangle source;
    Rectangle hitbox1;
    Rectangle hitbox2;
    Rectangle dest1;
    Rectangle dest2;

    void movePipe(float& dt){
      this->pos.x -= pipespeed * dt;
    }

    void updatePipe(float& dt){

            //TraceLog(LOG_INFO, "w=%f h=%f pos=%f,%f", width, height, pos.x, pos.y);
      dest1 = {this->pos.x,this->pos.y + gap, this->width, this->height};
      dest2 = {this->pos.x,this->pos.y - 4*gap, width, height};
      this->hitbox1 = {pos.x - hitW / 2, (pos.y - hitH/2) + gap, hitW, hitH};
      this->hitbox2 = {pos.x - hitW / 2, (pos.y - hitH/2) - 4*(gap), hitW, hitH};

      movePipe(dt);
      DrawTexturePro(this->pipeTex, {0, 0, -source.width, source.height}, this->dest2, this->origin, 180, WHITE);
      DrawTexturePro(this->pipeTex, this->source, this->dest1, this->origin, 0, WHITE);     


      if (this->pos.x < (0-this->size.x)){
        
        this->pos.x = 800;
        this->pos.y = distr(gen);
        this->scoreDebounce = false;

      }
    }

    Pipes(int xpos, Texture2D tex){
      this->pipeTex = tex;
      this->pos.x = xpos;
      this->ogX = xpos;
      this->pos.y = distr(gen);
      source = {0, 0, (float)pipeTex.width, (float)pipeTex.height};
      width = pipeTex.width * imgScale;
      height = pipeTex.height * imgScale;
      origin = {this->width / 2, this->height / 2};
    }

    void setPipeSpeed(float speed){
      this->pipespeed = speed;
    }

    int score(int x, int& score){
      if (this->pos.x <= x && this->scoreDebounce == false){
        scoreDebounce = true;
        return score + 1;
      }

      return score;
    }

    bool collidesWith(const Rectangle& other) const{
      return CheckCollisionRecs(other, hitbox1) || CheckCollisionRecs(other, hitbox2);
      
    }

    void reset(){
      pos.x = ogX;
      pipespeed = 200.0f;
      this->pos.y = distr(gen);
    }
};

std::vector<Pipes> pipesInit(Texture2D tex);

void reset( Player& p, std::vector<Pipes>& pipevector, int& score, bool& start, float& glps, float& speedtimer){
  for (size_t i = 0; i<pipevector.size(); i++){
    pipevector[i].reset();
  }
  p.reset();
  score = 0;
  start = false;
  glps = 300.0f;
  speedtimer = 0;
}

int main() {

  bool start = false;

  const int screenWidth = 800; //sets screen width to 800px
  const int screenHeight = 450; //sets screen height to 450px

  InitWindow(screenWidth, screenHeight, "Larpy Bird"); //InitWindow creates the window
  InitAudioDevice();
  SetTargetFPS(20000); // SetTargetFPS sets the fps to whatever int in the brackets

  Sound flapSound = LoadSound("resources/jump.wav");
  Sound deathSound = LoadSound("resources/death.wav");
                  
  Texture2D flappyTex = LoadTexture("resources/flappy.png");
  Texture2D pipeTex = LoadTexture("resources/pipe.png");
  Texture2D groundTex = LoadTexture("resources/ground.png");
  
  Image icon = LoadImage("resources/flappy.png");
  SetWindowIcon(icon);
  UnloadImage(icon);

  std::vector pipevector = pipesInit(pipeTex);

  Player player(200, 225, flappyTex, deathSound, flapSound);

  Ground ground(groundTex, 0.0f, 400.0f);
  
  
  float globalPipeSpeed= 300.0f;
  float speedTimer = 0.0f;
  int score = 0;
  
  float gravity = 2200.0f; 

  while (!WindowShouldClose()) { // "While the window shouldn't close then do whatever is inside it"
    
    float dt = GetFrameTime();

    speedTimer+= dt;

    BeginDrawing(); // brackets everything you draw in a frame 
      ClearBackground(SKYBLUE); //wipes the previous frame

      if (IsKeyPressed(KEY_R) && player.getDeath()){
        reset(player, pipevector, score, start, globalPipeSpeed, speedTimer);
      }
      if (player.getDeath() == false && start){
        for (size_t i = 0; i<pipevector.size(); i++){
          pipevector[i].updatePipe(dt);
          if (pipevector[i].collidesWith(player.getHitbox())){
            player.death(dt);
          }
          pipevector[i].setPipeSpeed(globalPipeSpeed);
          score = pipevector[i].score(player.pos.x, score);
        }
      if (roundTo(speedTimer, 0) == 3){
        speedTimer = 0.0f;
        globalPipeSpeed = globalPipeSpeed* 1.02;
      }

      }
      if (start){
        gravity=2200.0f;
      } else{
        DrawText("Press 'Up Arrow' to start",(screenWidth/2) - 200, (screenHeight/2) + 40, 32, BLACK);
        gravity = 0.0f;
      }
      if (player.getDeath()){
        DrawText("Press 'R' to restart",(screenWidth/2) - 160, (screenHeight/2), 32, BLACK);
      }
      player.updatePlayer(gravity, dt, start);
      DrawText(TextFormat("%d", score),(screenWidth/2) - 10, 76, 64, BLACK);
      ground.updateGround(dt);
    EndDrawing();
  }
  
  CloseWindow(); //cleans up
  return 0;
}

std::vector<Pipes> pipesInit(Texture2D tex){

  std::vector<Pipes> pipevector;

  Pipes pipe1(800, tex);
  Pipes pipe2(1100, tex);
  Pipes pipe3(1400, tex);
  Pipes pipe4(1700, tex);

  pipevector.push_back(pipe1);
  pipevector.push_back(pipe2);
  pipevector.push_back(pipe3);
  //pipevector.push_back(pipe4);

  return pipevector;
}
