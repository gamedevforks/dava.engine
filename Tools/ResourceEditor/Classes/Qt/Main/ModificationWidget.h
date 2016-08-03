#pragma once

#include <QWidget>
#include <QAbstractSpinBox>
#include <QLabel>

#include "Scene/SceneEditor2.h"
#include "Scene/SceneSignals.h"

class DAVAFloat32SpinBox;
class ModificationWidget : public QWidget
{
    Q_OBJECT

public:
    enum PivotMode : DAVA::uint32
    {
        PivotAbsolute,
        PivotRelative,
    };

    explicit ModificationWidget(QWidget* parent = nullptr);

    void SetPivotMode(PivotMode pivotMode);
    void SetTransformType(Selectable::TransformType modifMode);

    void ReloadValues();

public slots:
    void OnSnapToLandscapeChanged();

private slots:
    void OnSceneActivated(SceneEditor2* scene);
    void OnSceneDeactivated(SceneEditor2* scene);
    void OnSceneSelectionChanged(SceneEditor2* scene, const SelectableGroup* selected, const SelectableGroup* deselected);
    void OnSceneCommand(SceneEditor2* scene, const Command2* command, bool redo);

    void OnXChanged();
    void OnYChanged();
    void OnZChanged();

private:
    void ApplyValues(ST_Axis axis);

private:
    QLabel* xLabel = nullptr;
    QLabel* yLabel = nullptr;
    QLabel* zLabel = nullptr;
    DAVAFloat32SpinBox* xAxisModify = nullptr;
    DAVAFloat32SpinBox* yAxisModify = nullptr;
    DAVAFloat32SpinBox* zAxisModify = nullptr;
    SceneEditor2* curScene = nullptr;
    PivotMode pivotMode = PivotMode::PivotAbsolute;
    Selectable::TransformType modifMode = Selectable::TransformType::Disabled;
    bool groupMode = false;
};

class DAVAFloat32SpinBox : public QAbstractSpinBox
{
    Q_OBJECT

public:
    explicit DAVAFloat32SpinBox(QWidget* parent = nullptr);

    void showButtons(bool show);

    DAVA::float32 value() const;
    void setValue(DAVA::float32 val);
    void stepBy(int steps) override;

signals:
    void valueEdited();

public slots:
    void clear() override;

protected slots:
    void textEditingFinished();

private:
    bool eventFilter(QObject* object, QEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    StepEnabled stepEnabled() const override;

    DAVA::float32 originalValue = 0;

    static const int precision = 3;
    static const DAVA::float32 eps;

    bool hasButtons = true;
};
