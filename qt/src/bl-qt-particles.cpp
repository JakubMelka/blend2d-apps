#include "qblcanvas.h"

#include <stdlib.h>
#include <cmath>
#include <vector>

// ============================================================================
// [Particle]
// ============================================================================

struct Particle {
  BLPoint p;
  BLPoint v;
  int age;
  int category;
};

// ============================================================================
// [MainWindow]
// ============================================================================

class MainWindow : public QWidget {
  Q_OBJECT

public:
  QTimer _timer;
  QComboBox _rendererSelect;
  QCheckBox _limitFpsCheck;
  QCheckBox _colorsCheck;
  QSlider _countSlider;
  QBLCanvas _canvas;

  BLRandom _rnd;
  std::vector<Particle> _particles;
  int maxAge = 250;
  double radiusScale = 6;
  BLRgba32 colors[3] = { BLRgba32(0xFFFF7F00), BLRgba32(0xFFFF3F9F), BLRgba32(0xFF7F4FFF) };

  enum { kCategoryCount = 3 };

  MainWindow() {
    QVBoxLayout* vBox = new QVBoxLayout();
    vBox->setContentsMargins(0, 0, 0, 0);
    vBox->setSpacing(0);

    QGridLayout* grid = new QGridLayout();
    grid->setContentsMargins(5, 5, 5, 5);
    grid->setSpacing(5);

    _rendererSelect.addItem("Blend2D", QVariant(int(QBLCanvas::RendererB2D)));
    _rendererSelect.addItem("Qt", QVariant(int(QBLCanvas::RendererQt)));
    _limitFpsCheck.setText(QLatin1Literal("Limit FPS"));
    _limitFpsCheck.setChecked(true);

    _colorsCheck.setText(QLatin1Literal("Colors"));

    _countSlider.setMinimum(0);
    _countSlider.setMaximum(2000);
    _countSlider.setValue(500);
    _countSlider.setOrientation(Qt::Horizontal);

    connect(&_rendererSelect, SIGNAL(activated(int)), SLOT(onRendererChanged(int)));
    connect(&_limitFpsCheck, SIGNAL(stateChanged(int)), SLOT(onLimitFpsChanged(int)));

    grid->addWidget(new QLabel("Renderer:"), 0, 0);
    grid->addWidget(&_rendererSelect, 0, 1);
    grid->addWidget(&_limitFpsCheck, 0, 2);
    grid->addWidget(&_colorsCheck, 0, 3);
    grid->addWidget(&_countSlider, 0, 4);

    _canvas.onRenderB2D = std::bind(&MainWindow::onRenderB2D, this, std::placeholders::_1);
    _canvas.onRenderQt = std::bind(&MainWindow::onRenderQt, this, std::placeholders::_1);

    vBox->addItem(grid);
    vBox->addWidget(&_canvas);
    setLayout(vBox);

    connect(&_timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    onInit();
    onLimitFpsChanged(_limitFpsCheck.isChecked());
    _updateTitle();
  }

  void showEvent(QShowEvent* event) override { _timer.start(); }
  void hideEvent(QHideEvent* event) override { _timer.stop(); }
  void keyPressEvent(QKeyEvent* event) override {}

  void onInit() {
    _rnd.reset(1234);
  }

  Q_SLOT void onRendererChanged(int index) { _canvas.setRendererType(_rendererSelect.itemData(index).toInt()); }
  Q_SLOT void onLimitFpsChanged(int value) { _timer.setInterval(value ? 1000 / 60 : 2); }

  Q_SLOT void onTimer() {
    size_t i = 0;
    size_t j = 0;
    size_t count = _particles.size();

    double PI = 3.14159265359;
    BLMatrix2D m = BLMatrix2D::makeRotation(0.01);

    while (i < count) {
      Particle& p = _particles[i++];
      p.p += p.v;
      p.v = m.mapPoint(p.v);
      if (++p.age >= maxAge)
        continue;
      _particles[j++] = p;
    }
    _particles.resize(j);

    size_t maxParticles = size_t(_countSlider.value());
    size_t n = size_t(_rnd.nextDouble() * maxParticles / 60 + 0.95);

    for (i = 0; i < n; i++) {
      if (_particles.size() >= maxParticles)
        break;

      double angle = _rnd.nextDouble() * PI * 2.0;
      double speed = blMax(_rnd.nextDouble() * 2.0, 0.1);
      double aSin = std::sin(angle);
      double aCos = std::cos(angle);

      Particle part;
      part.p.reset();
      part.v.reset(aCos * speed, aSin * speed);
      part.age = int(blMin(_rnd.nextDouble(), 0.5) * maxAge);
      part.category = int(_rnd.nextDouble() * kCategoryCount);
      _particles.push_back(part);
    }

    _canvas.updateCanvas(true);
    _updateTitle();
  }

  void onRenderB2D(BLContext& ctx) noexcept {
    ctx.setFillStyle(BLRgba32(0xFF000000u));
    ctx.fillAll();

    double cx = _canvas.width() / 2;
    double cy = _canvas.height() / 2;

    if (_colorsCheck.isChecked()) {
      BLPath paths[kCategoryCount];

      for (Particle& part : _particles) {
        paths[part.category].addCircle(BLCircle(cx + part.p.x, cy + part.p.y, double(maxAge - part.age) / double(maxAge) * radiusScale));
      }

      ctx.setCompOp(BL_COMP_OP_PLUS);
      for (size_t i = 0; i < kCategoryCount; i++) {
        ctx.setFillStyle(colors[i]);
        ctx.fillPath(paths[i]);
      }
    }
    else {
      BLPath path;
      for (Particle& part : _particles) {
        path.addCircle(BLCircle(cx + part.p.x, cy + part.p.y, double(maxAge - part.age) / double(maxAge) * radiusScale));
      }

      ctx.setFillStyle(BLRgba32(0xFFFFFFFFu));
      ctx.fillPath(path);
    }
  }

  void onRenderQt(QPainter& ctx) noexcept {
    ctx.fillRect(0, 0, _canvas.width(), _canvas.height(), QColor(0, 0, 0));
    ctx.setRenderHint(QPainter::Antialiasing, true);

    double cx = _canvas.width() / 2;
    double cy = _canvas.height() / 2;

    if (_colorsCheck.isChecked()) {
      QPainterPath paths[kCategoryCount];

      for (Particle& part : _particles) {
        double r = double(maxAge - part.age) / double(maxAge) * radiusScale;
        double d = r * 2.0;
        paths[part.category].addEllipse(cx + part.p.x - r, cy + part.p.y - r, d, d);
      }

      ctx.setCompositionMode(QPainter::CompositionMode_Plus);
      for (size_t i = 0; i < kCategoryCount; i++) {
        paths[i].setFillRule(Qt::WindingFill);
        ctx.fillPath(paths[i], QBrush(QColor(qRgb(colors[i].r, colors[i].g, colors[i].b))));
      }
    }
    else {
      QPainterPath p;
      p.setFillRule(Qt::WindingFill);

      for (Particle& part : _particles) {
        double r = double(maxAge - part.age) / double(maxAge) * radiusScale;
        double d = r * 2.0;
        p.addEllipse(cx + part.p.x - r, cy + part.p.y - r, d, d);
      }

      ctx.fillPath(p, QBrush(QColor(qRgb(255, 255, 255))));
    }
  }

  void _updateTitle() {
    char buf[256];
    snprintf(buf, 256, "Particles Sample [%dx%d] [%d particles] [%.1f FPS]",
      _canvas.width(),
      _canvas.height(),
      int(_particles.size()),
      _canvas.fps());

    QString title = QString::fromUtf8(buf);
    if (title != windowTitle())
      setWindowTitle(title);
  }
};

// ============================================================================
// [Main]
// ============================================================================

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  MainWindow win;

  win.setMinimumSize(QSize(400, 320));
  win.resize(QSize(580, 520));
  win.show();

  return app.exec();
}

#include "bl-qt-particles.moc"
