#include "Matchmaker.h"
#include "Game.h"

Matchmaker::Matchmaker(QObject *parent) : QThread(parent) {
    connect(this, &Matchmaker::sendMessageToSocket, this, [](QTcpSocket *socket, QByteArray data) {
        if (socket->state() == QAbstractSocket::ConnectedState) {
            qDebug().noquote() << data << '\n';
            socket->write(data);
            socket->flush();
        } else {
            qDebug() << "Socket is not in connected state, cannot write data.";
        }
    });
}

Matchmaker::~Matchmaker() {
}

void Matchmaker::addPlayer(QTcpSocket *playerSocket, QString username) {
    QMutexLocker locker(&mutex);
    waitingPlayers.enqueue(playerSocket);
    playerUsernames[playerSocket] = username;
}

void Matchmaker::run() {
    while (true) {
        findMatch();
        msleep(100);
    }
}

void Matchmaker::findMatch() {
    QMutexLocker locker(&mutex);

    if (waitingPlayers.size() >= 2) {
        QTcpSocket *player1 = waitingPlayers.dequeue();
        QTcpSocket *player2 = waitingPlayers.dequeue();

        startMatch(player1, player2);
    }

    DatabaseManager::instance().cleanupConnection();
}

void Matchmaker::sendMatchStart(QTcpSocket *player1, QTcpSocket *player2, const QString &matchId) {
    // Construct the message for player1
    QJsonObject matchStartMessagePlayer1;
    matchStartMessagePlayer1["type"] = "find_match_ack";
    matchStartMessagePlayer1["status"] = "success";
    matchStartMessagePlayer1["message"] = "Match found successfully";
    matchStartMessagePlayer1["opponent"] = playerUsernames[player2];
    matchStartMessagePlayer1["match_id"] = matchId;
    matchStartMessagePlayer1["role"] = "WHITE";

    QJsonDocument docPlayer1(matchStartMessagePlayer1);
    QByteArray dataPlayer1 = docPlayer1.toJson(QJsonDocument::Indented); // JSON đẹp
    emit sendMessageToSocket(player1, dataPlayer1);

    // Construct the message for player2
    QJsonObject matchStartMessagePlayer2;
    matchStartMessagePlayer2["type"] = "find_match_ack";
    matchStartMessagePlayer2["status"] = "success";
    matchStartMessagePlayer2["message"] = "Match found successfully";
    matchStartMessagePlayer2["opponent"] = playerUsernames[player1];
    matchStartMessagePlayer2["match_id"] = matchId;
    matchStartMessagePlayer2["role"] = "BLACK";

    QJsonDocument docPlayer2(matchStartMessagePlayer2);
    QByteArray dataPlayer2 = docPlayer2.toJson(QJsonDocument::Indented); // JSON đẹp
    emit sendMessageToSocket(player2, dataPlayer2);

    // Log the match start
    qDebug() << "Match started between " << playerUsernames[player1]
             << " (WHITE) and " << playerUsernames[player2] << " (BLACK). Match ID: " << matchId;
}

QString Matchmaker::createMatchInDB(const QString &player1, const QString &player2) {
    QSqlDatabase db = DatabaseManager::instance().getDatabase();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "Database connection is invalid or not open.";
        return QString();  // Trả về rỗng nếu kết nối không hợp lệ
    }

    QString matchId = QUuid::createUuid().toString(QUuid::WithoutBraces);  // Tạo match_id

    QSqlQuery query(db);  // Khởi tạo QSqlQuery với QSqlDatabase của luồng hiện tại
    query.prepare("INSERT INTO matches (match_id, player1, player2, status, start_time) "
                  "VALUES (:match_id, :player1, :player2, 'ongoing', :start_time)");
    query.bindValue(":match_id", matchId);
    query.bindValue(":player1", player1);
    query.bindValue(":player2", player2);
    query.bindValue(":start_time", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        qWarning() << "Failed to insert match into database:" << query.lastError().text();
        return QString();  // Trả về rỗng nếu lỗi
    }

    return matchId;
}

void Matchmaker::startMatch(QTcpSocket *player1, QTcpSocket *player2) {
    // Lấy tên người chơi từ socket
    QString player1Name = playerUsernames[player1];
    QString player2Name = playerUsernames[player2];

    // Tạo match_id và lưu vào database
    QString matchId = createMatchInDB(player1Name, player2Name);
    if (matchId.isEmpty()) {
        qWarning() << "Failed to create match!";
        return;
    }

    // Thông báo bắt đầu trận đấu
    sendMatchStart(player1, player2, matchId);

    // Tạo luồng GameThread để xử lý trận đấu
    Game *game = new Game(player1, player2, matchId);  // Truyền match_id vào game
    game->start();  // Khởi chạy thread xử lý trận đấu

}

void Matchmaker::sendErrorResponse(QTcpSocket *clientSocket, const QString &errorMessage) {
    QJsonObject errorResponse;
    errorResponse["status"] = "error";
    errorResponse["message"] = errorMessage;

    QJsonDocument doc(errorResponse);
    QByteArray data = doc.toJson();

    clientSocket->write(data);
    qDebug() << "Error sent to client:" << errorMessage;
}
