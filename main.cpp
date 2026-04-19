#include <GL/freeglut.h>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <string>
#include <sstream>

// ---------------------------
// GAME STATE
// ---------------------------
int score = 0;
int timeLeft = 30;
bool gameOver = false;

const int HOLES = 6;
int activeHole = -1;
bool bunnyVisible = false;

// HOLE POSITIONS
float holeX[HOLES] = { -0.8f, 0.0f, 0.8f, -0.8f, 0.0f, 0.8f };
float holeY[HOLES] = { 0.5f, 0.5f, 0.5f, -0.35f, -0.35f, -0.35f };
float holeZ = 0.0f;

// WINDOW
int windowWidth = 800;
int windowHeight = 600;

// HAMMER / CURSOR
float mouseX = 0.0f;
float mouseY = 0.0f;
bool isSwinging = false;

// POPUP
float popupX = 0.0f, popupY = 0.0f;
int popupTimer = 0;

// MISS FLASH
int missFlash = 0;

// ---------------------------
// UTILS
// ---------------------------
void drawBitmapText(float x, float y, void* font, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

void drawStrokeTextCentered(float centerX, float y, float scale, const char* text) {
    float width = (float)glutStrokeLength(GLUT_STROKE_ROMAN, (const unsigned char*)text);
    float startX = centerX - (width * scale * 0.5f);

    glPushMatrix();
    glTranslatef(startX, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    for (const char* p = text; *p; ++p) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
    }

    glPopMatrix();
}

void drawCircle2D(float cx, float cy, float r, int segments = 64) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float a = 2.0f * 3.1415926f * i / segments;
        glVertex2f(cx + std::cos(a) * r, cy + std::sin(a) * r);
    }
    glEnd();
}

void screenToWorldOnBoard(int sx, int sy, float& wx, float& wy) {
    float nx = (2.0f * sx) / windowWidth - 1.0f;
    float ny = 1.0f - (2.0f * sy) / windowHeight;

    float aspect = (float)windowWidth / (float)windowHeight;
    float worldHeight = 2.2f;
    float worldWidth = worldHeight * aspect;

    wx = nx * (worldWidth / 2.0f);
    wy = ny * (worldHeight / 2.0f);
}

float distance2D(float x1, float y1, float x2, float y2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return std::sqrt(dx * dx + dy * dy);
}

// ---------------------------
// SCENE DRAWING
// ---------------------------
void drawGround() {
    glDisable(GL_LIGHTING);

    glColor3f(0.36f, 0.67f, 0.27f);
    glBegin(GL_QUADS);
    glVertex3f(-2.4f, -1.2f, -0.15f);
    glVertex3f(2.4f, -1.2f, -0.15f);
    glVertex3f(2.4f, 1.2f, -0.15f);
    glVertex3f(-2.4f, 1.2f, -0.15f);
    glEnd();

    glEnable(GL_LIGHTING);
}

void drawHole3D(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, holeZ);

    glEnable(GL_LIGHTING);

    // outer rim
    glColor3f(0.50f, 0.29f, 0.14f);
    glutSolidTorus(0.025, 0.12, 22, 28);

    // inner ring
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.03f);
    glColor3f(0.22f, 0.10f, 0.05f);
    glutSolidTorus(0.012, 0.082, 18, 22);
    glPopMatrix();

    // dark pit
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.07f);
    glScalef(1.0f, 1.0f, 0.55f);
    glColor3f(0.10f, 0.04f, 0.02f);
    glutSolidSphere(0.10, 24, 24);
    glPopMatrix();

    glPopMatrix();
}

void drawBunny3D(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.08f);

    glEnable(GL_LIGHTING);

    // body
    glPushMatrix();
    glScalef(1.0f, 0.95f, 1.0f);
    glColor3f(1.0f, 0.72f, 0.82f);
    glutSolidSphere(0.075, 24, 24);
    glPopMatrix();

    // belly
    glPushMatrix();
    glTranslatef(0.0f, -0.005f, 0.05f);
    glScalef(0.65f, 0.58f, 0.45f);
    glColor3f(1.0f, 0.85f, 0.91f);
    glutSolidSphere(0.07, 20, 20);
    glPopMatrix();

    // head
    glPushMatrix();
    glTranslatef(0.0f, 0.10f, 0.01f);
    glColor3f(1.0f, 0.80f, 0.88f);
    glutSolidSphere(0.058, 24, 24);
    glPopMatrix();

    // left ear
    glPushMatrix();
    glTranslatef(-0.028f, 0.18f, 0.0f);
    glScalef(0.45f, 1.35f, 0.45f);
    glColor3f(1.0f, 0.78f, 0.86f);
    glutSolidSphere(0.04, 18, 18);
    glPopMatrix();

    // right ear
    glPushMatrix();
    glTranslatef(0.028f, 0.18f, 0.0f);
    glScalef(0.45f, 1.35f, 0.45f);
    glColor3f(1.0f, 0.78f, 0.86f);
    glutSolidSphere(0.04, 18, 18);
    glPopMatrix();

    // tail
    glPushMatrix();
    glTranslatef(-0.035f, -0.02f, -0.05f);
    glColor3f(1.0f, 0.92f, 0.96f);
    glutSolidSphere(0.02, 16, 16);
    glPopMatrix();

    // face details in local coordinates
    glDisable(GL_LIGHTING);

    // cheeks
    glColor3f(1.0f, 0.55f, 0.72f);
    drawCircle2D(-0.022f, 0.095f, 0.010f);
    drawCircle2D(0.022f, 0.095f, 0.010f);

    // eyes
    glColor3f(0.1f, 0.1f, 0.1f);
    drawCircle2D(-0.018f, 0.115f, 0.008f);
    drawCircle2D(0.018f, 0.115f, 0.008f);

    // nose
    glColor3f(0.95f, 0.35f, 0.55f);
    drawCircle2D(0.0f, 0.10f, 0.0065f);

    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void drawHammer3D() {
    glPushMatrix();

    // hammer follows mouse
    glTranslatef(mouseX, mouseY, 0.45f);

    // swing rotation
    float angle = isSwinging ? -55.0f : -18.0f;
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glEnable(GL_LIGHTING);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    // handle
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);  // z-axis cylinder points upward in y
    glTranslatef(0.0f, 0.0f, -0.16f);
    glColor3f(0.60f, 0.35f, 0.16f);
    gluCylinder(quad, 0.012, 0.010, 0.16, 16, 4);
    glPopMatrix();

    // top connector
    glPushMatrix();
    glTranslatef(0.0f, 0.012f, 0.0f);
    glScalef(0.020f, 0.040f, 0.028f);
    glColor3f(0.55f, 0.30f, 0.12f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // head at top
    glPushMatrix();
    glTranslatef(0.0f, 0.050f, 0.0f);
    glScalef(0.11f, 0.04f, 0.05f);
    glColor3f(0.92f, 0.22f, 0.28f);
    glutSolidCube(1.0f);
    glPopMatrix();

    gluDeleteQuadric(quad);
    glPopMatrix();
}

// ---------------------------
// 2D HUD
// ---------------------------
void begin2DOverlay() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
}

void end2DOverlay() {
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

void drawHUD() {
    begin2DOverlay();

    // centered small title shadow
    glLineWidth(3.0f);
    glColor3f(0.25f, 0.02f, 0.18f);
    drawStrokeTextCentered(0.01f, 0.845f, 0.00095f, "WHACK A BUNNY");

    // main title
    glColor3f(1.0f, 0.55f, 0.82f);
    drawStrokeTextCentered(0.0f, 0.855f, 0.00095f, "WHACK A BUNNY");

    // score and time
    glColor3f(0.08f, 0.08f, 0.08f);

    std::stringstream ss1, ss2;
    ss1 << "Score: " << score;
    ss2 << "Time: " << timeLeft;

    drawBitmapText(-0.94f, 0.90f, GLUT_BITMAP_HELVETICA_18, ss1.str());
    drawBitmapText(0.67f, 0.90f, GLUT_BITMAP_HELVETICA_18, ss2.str());

    // +10 popup
    if (popupTimer > 0) {
        glColor3f(1.0f, 0.82f, 0.18f);
        drawBitmapText(popupX, popupY, GLUT_BITMAP_HELVETICA_18, "+10");
        popupY += 0.012f;
        popupTimer--;
    }

    // game over
    if (gameOver) {
        glColor3f(0.20f, 0.0f, 0.0f);
        drawBitmapText(-0.18f, 0.08f, GLUT_BITMAP_TIMES_ROMAN_24, "GAME OVER");

        std::stringstream finalText;
        finalText << "Final Score: " << score;
        drawBitmapText(-0.20f, -0.02f, GLUT_BITMAP_HELVETICA_18, finalText.str());
        drawBitmapText(-0.35f, -0.14f, GLUT_BITMAP_HELVETICA_18, "Press R to restart or ESC to quit");
    }

    end2DOverlay();
}

// ---------------------------
// GAME LOGIC
// ---------------------------
void spawn(int value) {
    if (gameOver) return;

    activeHole = rand() % HOLES;
    bunnyVisible = true;

    glutTimerFunc(800, [](int) {
        bunnyVisible = false;
        }, 0);

    glutTimerFunc(1200, spawn, 0);
}

void gameTimer(int value) {
    if (gameOver) return;

    timeLeft--;

    if (timeLeft <= 0) {
        timeLeft = 0;
        gameOver = true;
        bunnyVisible = false;
    }
    else {
        glutTimerFunc(1000, gameTimer, 0);
    }

    glutPostRedisplay();
}

void resetGame() {
    score = 0;
    timeLeft = 30;
    gameOver = false;
    activeHole = -1;
    bunnyVisible = false;
    isSwinging = false;
    popupTimer = 0;
    missFlash = 0;

    spawn(0);
    glutTimerFunc(1000, gameTimer, 0);
}

// ---------------------------
// CALLBACKS
// ---------------------------
void display() {
    if (missFlash > 0) {
        glClearColor(1.0f, 0.35f, 0.35f, 1.0f);
        missFlash--;
    }
    else {
        glClearColor(0.70f, 0.90f, 1.0f, 1.0f);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0.0, 0.0, 2.6,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0);

    GLfloat light0Pos[] = { 1.5f, 1.8f, 2.2f, 1.0f };
    GLfloat light1Pos[] = { -1.8f, 0.8f, 1.4f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);

    drawGround();

    for (int i = 0; i < HOLES; i++) {
        drawHole3D(holeX[i], holeY[i]);
    }

    if (bunnyVisible && activeHole != -1) {
        drawBunny3D(holeX[activeHole], holeY[activeHole]);
    }

    drawHammer3D();
    drawHUD();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;

    windowWidth = w;
    windowHeight = h;

    float ratio = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(60.0, ratio, 0.1, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int stateClick, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && stateClick == GLUT_DOWN && !gameOver) {
        isSwinging = true;
        glutTimerFunc(120, [](int) {
            isSwinging = false;
            }, 0);

        float wx, wy;
        screenToWorldOnBoard(x, y, wx, wy);

        if (bunnyVisible && activeHole != -1) {
            float dist = distance2D(wx, wy, holeX[activeHole], holeY[activeHole]);

            if (dist < 0.16f) {
                score += 10;
                bunnyVisible = false;

                float aspect = (float)windowWidth / (float)windowHeight;
                float worldHeight = 2.2f;
                float worldWidth = worldHeight * aspect;

                popupX = (holeX[activeHole] / (worldWidth / 2.0f));
                popupY = (holeY[activeHole] / (worldHeight / 2.0f)) + 0.16f;
                popupTimer = 30;

                glutPostRedisplay();
                return;
            }
        }

        missFlash = 8;
    }
}

void passiveMotion(int x, int y) {
    float wx, wy;
    screenToWorldOnBoard(x, y, wx, wy);

    mouseX = wx;
    mouseY = wy;
}

void keyboard(unsigned char key, int, int) {
    if (key == 27) {
        std::exit(0);
    }
    if ((key == 'r' || key == 'R') && gameOver) {
        resetGame();
    }
}

void idle() {
    glutPostRedisplay();
}

// ---------------------------
// INIT
// ---------------------------
void init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat globalAmbient[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

    GLfloat light0Pos[] = { 1.5f, 1.8f, 2.2f, 1.0f };
    GLfloat light0Diffuse[] = { 0.95f, 0.92f, 0.88f, 1.0f };
    GLfloat light0Specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light0Ambient[] = { 0.20f, 0.18f, 0.18f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0Specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0Ambient);

    GLfloat light1Pos[] = { -1.8f, 0.8f, 1.4f, 1.0f };
    GLfloat light1Diffuse[] = { 0.35f, 0.40f, 0.45f, 1.0f };
    GLfloat light1Specular[] = { 0.15f, 0.15f, 0.20f, 1.0f };
    GLfloat light1Ambient[] = { 0.08f, 0.08f, 0.10f, 1.0f };

    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1Specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1Ambient);

    GLfloat matSpecular[] = { 0.45f, 0.45f, 0.45f, 1.0f };
    GLfloat matShininess[] = { 32.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
}

// ---------------------------
// MAIN
// ---------------------------
int main(int argc, char** argv) {
    srand((unsigned int)time(0));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Whack-a-Bunny OpenGL");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMotion);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    spawn(0);
    glutTimerFunc(1000, gameTimer, 0);

    glutMainLoop();
    return 0;
}