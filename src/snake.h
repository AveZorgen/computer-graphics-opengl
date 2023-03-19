#ifndef SNAKE_H
#define SNAKE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "rendering/mesh.h"
#include "rendering/shader.h"

const int WIDTH = 20;
const int HEIGHT = 20;

enum Snake_Movement {
    S_FORWARD,
    S_BACKWARD,
    S_LEFT,
    S_RIGHT
};

struct Model {
    Mesh mesh;
    Shader shader;
};

class Snake {
    glm::vec2 food;
    glm::vec2 snake[100];
    int snake_size = 5;
    int score = 0;
    bool game_over = true;
    void checkCollides() {
        // Check if the snake has collided with the game board walls
        if (snake[0].x <= 0 || snake[0].x >= WIDTH - 1 || snake[0].y <= 0 || snake[0].y >= HEIGHT - 1)
        {
            game_over = true;
        }
        // Check if the snake has collided with itself
        for (int i = 1; i < snake_size; i++)
        {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
            {
                game_over = true;
            }
        }
        // Check if the snake has collided with food
        if (snake[0].x == food.x && snake[0].y == food.y)
        {
            // Increase the snake's length and score
            snake_size++;
            score++;
            // Generate new food position
            food.x = rand() % (WIDTH - 2) + 1;
            food.y = rand() % (HEIGHT - 2) + 1;
            checkCollides();
        }
    }
public:
    bool isOvered() { return game_over; }
    void init() {
        food.x = rand() % (WIDTH - 2) + 1;
        food.y = rand() % (HEIGHT - 2) + 1;
        // std::cout << glm::to_string(food) << " " << game_over << std::endl;

        // Initialize the snake
        snake[0].x = WIDTH / 2;
        snake[0].y = HEIGHT / 2;
        for (int i = 1; i < snake_size; i++)
        {
            snake[i].x = snake[0].x + i;
            snake[i].y = snake[0].y;
        }
        checkCollides();
        game_over = false;
    }

    void update(Snake_Movement direction) {
        for (int i = snake_size - 1; i > 0; i--)
        {
            snake[i] = snake[i-1];
        }
        // Change the direction of the snake based on user input
        switch (direction)
        {
        case S_FORWARD: // Up
            snake[0].y--;
            break;
        case S_BACKWARD: // Down
            snake[0].y++;
            break;
        case S_LEFT: // Left
            snake[0].x--;
            break;
        case S_RIGHT: // Right
            snake[0].x++;
            break;
        }
        checkCollides();
    }


    void draw_blocks(Mesh mesh, Shader& shader) {
        if (game_over) return;
        glm::mat4 model = glm::mat4(1.0f);
        for (int i = 0; i < WIDTH; i++) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.0f, 0.0f));
            shader.setMat4("model", model); mesh.Draw();
            model = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.0f, HEIGHT - 1));
            shader.setMat4("model", model); mesh.Draw();
        }
        for (int j = 1; j < HEIGHT - 1; j++) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, j));
            shader.setMat4("model", model); mesh.Draw();
            model = glm::translate(glm::mat4(1.0f), glm::vec3(WIDTH - 1, 0.0f, j));
            shader.setMat4("model", model); mesh.Draw();
        }
    }

    void draw_snake(Mesh mesh, Shader& shader) {
        glm::mat4 model = glm::mat4(1.0f);
        for (int i = 0; i < snake_size; i++) {
            model = glm::translate(glm::mat4(1.0f), glm::vec3(snake[i].x, 0.0f, snake[i].y));
            shader.setMat4("model", model); mesh.Draw();
        }
    }

    void draw_food(Mesh mesh, Shader& shader) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(food.x, 0.0f, food.y));
        shader.setMat4("model", model); mesh.Draw();
    }
};

#endif