#include "BeastDialog.h"

#include <QEventLoop>
#include <QDir>
#include <QFileDialog>

#include "Classes/Qt/Scene/SceneEditor2.h"


BeastDialog::BeastDialog( QWidget *parent )
    : QWidget( parent, Qt::Window | Qt::CustomizeWindowHint | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint )
    , ui(new Ui::BeastDialog())
    , scene(NULL)
    , result(false)
{
    ui->setupUi(this);

    setWindowModality( Qt::WindowModal );

    connect( ui->start, SIGNAL( clicked() ), SLOT( OnStart() ) );
    connect( ui->cancel, SIGNAL( clicked() ), SLOT( OnCancel() ) );
    connect( ui->browse, SIGNAL( clicked() ), SLOT( OnBrowse() ) );
    connect( ui->output, SIGNAL( textChanged( const QString& ) ), SLOT( OnTextChanged() ) );
}

BeastDialog::~BeastDialog()
{
}

void BeastDialog::SetScene(SceneEditor2 *_scene)
{
    scene = _scene;
}

bool BeastDialog::Exec(QWidget *parent)
{
    DVASSERT(scene);

    ui->scenePath->setText( QDir::toNativeSeparators( GetDefaultPath() ) );
    ui->output->setText( "lightmaps" );

    QEventLoop *loop = new QEventLoop(this);

    connect( ui->start, SIGNAL( clicked() ), loop, SLOT( quit() ) );
    connect( ui->cancel, SIGNAL( clicked() ), loop, SLOT( quit() ) );

    show();
    loop->exec();
    hide();
    loop->deleteLater();

    return result;
}

void BeastDialog::OnStart()
{
    result = true;
}

void BeastDialog::OnCancel()
{
    result = false;
}

void BeastDialog::OnBrowse()
{
    const QString path = QFileDialog::getExistingDirectory( this, QString(), GetPath(), QFileDialog::ShowDirsOnly );
    if ( !path.isEmpty() )
    {
        SetPath( path );
    }
}

void BeastDialog::OnTextChanged()
{
    const QString text = QString( "Output path: %1" ).arg( QDir::toNativeSeparators( GetPath() ) );
    ui->output->setToolTip( text );
}

void BeastDialog::closeEvent( QCloseEvent * event )
{
    Q_UNUSED(event);
    ui->cancel->click();
}

QString BeastDialog::GetPath() const
{
    const QString root = ui->scenePath->text();
    const QString output = ui->output->text();
    QString path = QString( "%1/" ).arg( root );
    if ( !output.isEmpty() )
    {
        path += QString( "%1/" ).arg( output );
    }

    return path;
}

QString BeastDialog::GetDefaultPath() const
{
    if ( !scene )
        return QString();

    const QString absoluteFilePath = scene->GetScenePath().GetAbsolutePathname().c_str();
    const QFileInfo fileInfo( absoluteFilePath );
    const QString dir = fileInfo.absolutePath();

    return dir;
}

void BeastDialog::SetPath( const QString& path )
{
    const QString mandatory = QDir::fromNativeSeparators( GetDefaultPath() );
    const QString pathRoot = path.left(mandatory.length());
    if ( QDir( mandatory + '/' ) != QDir( pathRoot + '/' ) )
    {
        ui->output->setText( QString() );
        return;
    }

    const QString relative = path.mid(mandatory.length() + 1);
    ui->output->setText( QDir::toNativeSeparators( relative ) );
}
