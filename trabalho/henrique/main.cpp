// OpenGL / FreeGLUT - Nome interativo: "henrique"
// F1–F6 completos, com preservação em reshape (PARTE 1).
// Compilar (Linux):  g++ main.cpp -o app -lglut -lGLU -lGL
// Compilar (Windows MinGW): g++ main.cpp -o app -lfreeglut -lopengl32 -lglu32
// Se usar MSVC, adicione freeglut e opengl32 ao projeto.

#include <GL/freeglut.h>   // ou <GL/glut.h> conforme seu setup
#include <cmath>
#include <string>
using std::string;

// ---------- Estado da cena ----------
static string NAME_STR = "henrique";

// Transformações controladas pelo teclado
static float g_scale = 0.9f;          // escala uniforme (tem piso p/ não “sumir”)
static float g_angleDeg = 0.0f;       // rotação em graus (em torno do eixo Z)
static float g_tx = 0.0f, g_ty = 0.0f;// translação

// Janela / projeção
static int   g_winW = 900, g_winH = 600;
static float g_aspect = (float)g_winW / (float)g_winH;

// Métricas do texto em unidades de fonte "stroke" do GLUT
struct StrokeMetrics { float width; float height; };
static StrokeMetrics g_baseMetrics;

// Altura “canônica” da fonte stroke (aprox. para GLUT_STROKE_ROMAN)
static constexpr float STROKE_EM_HEIGHT = 119.05f; // valor padrão conhecido

// Limites do mundo (projeção ortográfica): X in [-aspect, +aspect], Y in [-1, +1]
static inline float worldMinX(){ return -g_aspect; }
static inline float worldMaxX(){ return  g_aspect; }
static inline float worldMinY(){ return -1.0f; }
static inline float worldMaxY(){ return  1.0f; }

// Passos e limites
static constexpr float SCALE_MIN = 0.15f;     // F4: impedir sumir com '-'
static constexpr float SCALE_STEP = 0.05f;
static constexpr float ROT_STEP_DEG = 5.0f;
static constexpr float MOVE_STEP = 0.08f;     // passo de WASD (em coords de mundo)

// ---------- Utilitário: mede largura total do nome na fonte stroke ----------
static StrokeMetrics measureStrokeString(const string& s)
{
    float w = 0.0f;
    for(char c : s) {
        w += glutStrokeWidth(GLUT_STROKE_ROMAN, c);
    }
    // altura base aproximada da fonte stroke
    return { w, STROKE_EM_HEIGHT };
}

// ---------- Desenho do nome em (0,0), sem transformações globais ----------
// Usamos a fonte stroke (linhas) → primitiva gráfica.
static void drawNameStroke(const string& s, float strokeScale)
{
    glPushMatrix();
    glScalef(strokeScale, strokeScale, 1.0f); // normaliza para altura ≈ 1.0 em coords de mundo

    for(char c : s) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }
    glPopMatrix();
}

// ---------- AABB do nome (em espaço do modelo ANTES da escala “global”) ----------
// Modelo base: desenhamos o texto começando em (0,0), com altura ≈ 1.0 após strokeScale.
// Logo, a caixa-alvo “model-space” é: [0, baseWidth*strokeScale] x [0, 1]
static void getModelAABB(float strokeScale, float& minx, float& miny, float& maxx, float& maxy)
{
    minx = 0.0f;
    miny = 0.0f;
    maxx = g_baseMetrics.width * strokeScale;
    maxy = 1.0f;
}

// ---------- Aplica T*R*S na AABB: gera AABB em espaço de mundo ----------
static void getWorldAABB(float strokeScale, float& outMinX, float& outMinY, float& outMaxX, float& outMaxY)
{
    // Pega AABB no model-space (após “strokeScale” local, antes da escala global/rot/trans)
    float minx, miny, maxx, maxy;
    getModelAABB(strokeScale, minx, miny, maxx, maxy);

    // Cantos da AABB base (model-space)
    float corners[4][2] = {
        {minx, miny}, {maxx, miny}, {maxx, maxy}, {minx, maxy}
    };

    // Monta a transformação global: S(g_scale), R(g_angleDeg), T(g_tx,g_ty)
    float a = g_angleDeg * 3.14159265f / 180.0f;
    float c = cosf(a), s = sinf(a);

    // varre cantos e calcula min/max em mundo
    outMinX = outMinY =  1e9f;
    outMaxX = outMaxY = -1e9f;

    for (int i = 0; i < 4; ++i)
    {
        float x = corners[i][0];
        float y = corners[i][1];

        // aplica escala global
        x *= g_scale;
        y *= g_scale;

        // aplica rotação Z
        float xr =  c*x - s*y;
        float yr =  s*x + c*y;

        // aplica translação
        xr += g_tx;
        yr += g_ty;

        if (xr < outMinX) outMinX = xr;
        if (xr > outMaxX) outMaxX = xr;
        if (yr < outMinY) outMinY = yr;
        if (yr > outMaxY) outMaxY = yr;
    }
}

// ---------- Clampa a translação para manter TODO o nome dentro da janela ----------
static void clampTranslationToView(float strokeScale)
{
    float minX, minY, maxX, maxY;
    getWorldAABB(strokeScale, minX, minY, maxX, maxY);

    float dx = 0.0f, dy = 0.0f;

    if (minX < worldMinX()) dx += (worldMinX() - minX);
    if (maxX > worldMaxX()) dx += (worldMaxX() - maxX);
    if (minY < worldMinY()) dy += (worldMinY() - minY);
    if (maxY > worldMaxY()) dy += (worldMaxY() - maxY);

    g_tx += dx;
    g_ty += dy;
}

// ---------- Display ----------
static void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // cor (linhas) — branca
    glColor3f(1.f, 1.f, 1.f);

    // strokeScale local p/ normalizar a altura da fonte para ~1.0
    float strokeScale = 1.0f / STROKE_EM_HEIGHT;

    // garante que, após qualquer mudança externa, o texto está visível
    clampTranslationToView(strokeScale);

    // Aplica transformações globais: T * R * S
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(g_tx, g_ty, 0.0f);
    glRotatef(g_angleDeg, 0.0f, 0.0f, 1.0f);
    glScalef(g_scale, g_scale, 1.0f);

    // Desenha a partir de (0,0)
    drawNameStroke(NAME_STR, strokeScale);

    glutSwapBuffers();
}

// ---------- Reshape (PARTE 1: mantém desenho correto ao redimensionar) ----------
static void reshape(int w, int h)
{
    g_winW = (w > 1) ? w : 1;
    g_winH = (h > 1) ? h : 1;
    g_aspect = (float)g_winW / (float)g_winH;

    glViewport(0, 0, g_winW, g_winH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Projeção ortográfica dependente do aspecto da janela
    glOrtho(-g_aspect, +g_aspect, -1.0, +1.0, -1.0, +1.0);

    glutPostRedisplay();
}

// ---------- Teclado (F3, F5, F6 + F4) ----------
static void keyboard(unsigned char key, int, int)
{
    bool changed = false;

    switch (key)
    {
        // Esc para sair
        case 27: // ESC
            glutLeaveMainLoop();
            return;

        // Escala (F3) com limite inferior (F4)
        case '+':
        case '=': // muitos layouts usam Shift+=
            g_scale += SCALE_STEP;
            changed = true;
            break;
        case '-':
        case '_':
        {
            float newScale = g_scale - SCALE_STEP;
            if (newScale < SCALE_MIN) newScale = SCALE_MIN; // F4
            g_scale = newScale;
            changed = true;
            break;
        }

        // Rotação (F5): anti-horário (q), horário (e)
        case 'q':
        case 'Q':
            g_angleDeg += ROT_STEP_DEG;
            if (g_angleDeg >= 360.0f) g_angleDeg -= 360.0f;
            changed = true;
            break;
        case 'e':
        case 'E':
            g_angleDeg -= ROT_STEP_DEG;
            if (g_angleDeg <= -360.0f) g_angleDeg += 360.0f;
            changed = true;
            break;

        // Translação (F6): wasd
        case 'w':
        case 'W':
            g_ty += MOVE_STEP;
            changed = true;
            break;
        case 's':
        case 'S':
            g_ty -= MOVE_STEP;
            changed = true;
            break;
        case 'a':
        case 'A':
            g_tx -= MOVE_STEP;
            changed = true;
            break;
        case 'd':
        case 'D':
            g_tx += MOVE_STEP;
            changed = true;
            break;
    }

    if (changed)
    {
        // Após qualquer transformação, clampa para manter TODO o nome visível (F6).
        float strokeScale = 1.0f / STROKE_EM_HEIGHT;
        clampTranslationToView(strokeScale);
        glutPostRedisplay();
    }
}

// ---------- Setup ----------
static void initGL()
{
    glClearColor(0.08f, 0.09f, 0.12f, 1.0f); // fundo escuro “bonitinho”
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(2.0f);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
    glutInitWindowSize(g_winW, g_winH);
    glutCreateWindow("CG - Transformacoes | Nome: henrique");

    initGL();

    // mede largura/altura base do texto (em unidades da fonte stroke)
    g_baseMetrics = measureStrokeString(NAME_STR);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
#ifdef _FREEGLUT_EXT_H_
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
#endif

    glutMainLoop();
    return 0;
}
