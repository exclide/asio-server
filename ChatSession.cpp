//
// Created by asd on 23-Oct-23.
//

#include "ChatSession.h"
#include "ChatRoom.h"

ChatSession::ChatSession(tcp::socket socket, const std::shared_ptr <ChatRoom> &room)
    : socket(std::move(socket)), room(room) {
}

ChatSession::~ChatSession() {
    room->Leave(this);
}


void ChatSession::Start() {
    room->Join(this);

    //первый вызов выполнится вне стрэнда, экзекутора ассоциорованному с текущим контекстом (сокетом)
    //поэтому закинем сразу на стрэнд
    boost::asio::dispatch(
            socket.get_executor(),
            [self = shared_from_this()]() {
                self->DoRead();
            });
}

void ChatSession::DoRead() {
    data.clear();

    std::cout << "async_read call\n";

    boost::asio::async_read_until(
            socket,
            boost::asio::dynamic_buffer(data),
            "\n",
            [self = shared_from_this()](const error_code& err, std::size_t bytes) {
                if (!err) {
                    self->room->Send(self->data);
                    self->DoRead();
                }
            }
    );

    std::cout << "async_read call exit\n";
}

void ChatSession::StartSend(const std::shared_ptr<std::string>& msg) {
    //изначально будет вызван с другого стрэнда из ChatRoom,
    //отдадим на выполнение стрэнду, ассоцированному с нашим сокетом и сразу выйдем
    boost::asio::post(
            socket.get_executor(),
            [self = shared_from_this(), msg]() { //тут важна копия msg, иначе ref_count мог бы достичь 0
                self->Send(msg);
            });
}


void ChatSession::Send(const std::shared_ptr<std::string>& msg) {
    sendq.push(msg); //нужно сохранить сообщение в текущий контекст (объект), чтобы не самоудалилось
    //при этом следующий Send(msg) может прийти до того, как мы успеем отправить msg текущего контекста,
    //поэтому нужна очередь

    if (sendq.size() > 1) { //уже пишем
        return;
    }

    DoWrite();
}

void ChatSession::DoWrite() {
    boost::asio::async_write(
            socket,
            boost::asio::buffer(*sendq.front()),
            [self = shared_from_this()] (const error_code& err, std::size_t bytes) {
                if (!err) {
                    self->sendq.pop(); //отправили сообщение, удалим из очереди

                    if (!self->sendq.empty()) { //в очереди остались сообщения
                        self->DoWrite();
                    }
                }
            });
}