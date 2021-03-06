#include "ktelephonemanager.h"
#include "ktelephone.h"
#include "ui_ktelephone.h"

#include <QDebug>

KTelephone::KTelephone(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ktelephone)
{
  ui->setupUi(this);

  this->setAttribute(Qt::WA_DeleteOnClose);

  statusLabel = new QLabel(this);
  ui->statusBar->addPermanentWidget(statusLabel);

  new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Comma), this,
                SLOT(actionPreferences()));
  new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this,
                SLOT(close()));

  connect(ui->actionPreferences,
          SIGNAL(triggered()), this,
          SLOT(actionPreferences()));
  connect(ui->actionAbout,
          SIGNAL(triggered()), this,
          SLOT(actionAbout()));
  connect(ui->statusComboBox,
          SIGNAL(currentIndexChanged(int)), this,
          SLOT(changeStatus(int)));
  connect(ui->sipInput,
          SIGNAL(returnPressed()), this,
          SLOT(actionOutboundCall()));
}

KTelephone::~KTelephone()
{
  mManager->getUserAgentManager()->removeUserAgent(mTelephone.username);
  delete ui;
}

void KTelephone::setManager(KTelephoneManager *manager)
{
  mManager = manager;
}

KTelephoneManager* KTelephone::getManager()
{
  return mManager;
}

void KTelephone::changeRegistrationStatus(bool status)
{
  static const char* statusNames[]  = { "Unregistered",
                                        "Registered",
                                        nullptr };
  disconnect(ui->statusComboBox,
             SIGNAL(currentIndexChanged(int)), 0, 0);
  statusMessage(statusNames[status]);
  ui->statusComboBox->setCurrentIndex(!status);
  connect(ui->statusComboBox,
          SIGNAL(currentIndexChanged(int)), this,
          SLOT(changeStatus(int)));
}

void KTelephone::statusMessage(QString message)
{
  statusLabel->setText(message);
}

void KTelephone::setTelephone(Telephone_t telephone)
{
  mTelephone = telephone;
  QString title = "";
  title.append(mTelephone.username);
  if (!mTelephone.description.isEmpty()) {
    title.append(" - ");
    title.append(mTelephone.description);
  }
  this->setWindowTitle(title);
}

void KTelephone::actionInboundCall(MyCall* call)
{
  KTelephoneCall *dialog = new KTelephoneCall(this, "inbound", mTelephone.username);
  dialog->setInstance(call);
  call->setInstance(dialog);
  dialog->show();
}

void KTelephone::actionOutboundCall()
{
  const QString dest = ui->sipInput->text();
  if (!dest.isEmpty()) {
    MyCall* call = mManager->getUserAgentManager()->placeCall(mTelephone.username, dest);
    KTelephoneCall *dialog = new KTelephoneCall(this, "outbound", mTelephone.username);
    dialog->setInstance(call);
    call->setInstance(dialog);
    dialog->show();
  }
}

void KTelephone::actionPreferences()
{
  mManager->openPreferences();
}

void KTelephone::actionAbout()
{
  qDebug() << "about";
}

void KTelephone::changeStatus(int index)
{
  switch (index) {
    case 0:
      mManager->getUserAgentManager()->setRegister(mTelephone.username, true);
      break;
    case 1:
      mManager->getUserAgentManager()->setRegister(mTelephone.username, false);
      break;
  }
}
