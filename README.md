# Solution
Клиент-серверное решение базируется на асинхронных сокетах и ***stackless coroutines*** из **Boost**.
Место в коде, где реализуется *stackless coroutines*, легко обнаруживается по слову (макросу) `reenter`.

Протокол общения основан на JSON. Общий формат сообщений:
```
{
    "op": "<operation_name>",
    "payload": {
        ........
    }
}
```
Всего существует два запроса клиента с соответствующеми ответами севера:

```
#
# Greeting message
#

# Request:
{
    "op": "Greeting",
    "payload": {
        "client_id": "<client_id>",
        "tokens_expected": <tokens_expected>
    }
}

# Response:
{
    "op": "Ready",
    "payload": {}
}
```

```
#
# Token message
#

# Request:
{
    "op": "Token",
    "payload": {
        "token": "<token>",
    }
}

# Response:
{
    "op": "Accepted",
    "payload": {
        "token": "<token>",
    }
}
```

## Details
По условиям тестового задания каждый обработчик содержит таблицу содержащую поля:
- **id** - клиента
- **ip:port** - клиета
- **expected** - ожидаемое количество токенов
- **received** - количество принятых токенов

Не совсем понятен момент с полями **expected** и **received**. По идее, это данные относящиеся к конкретному клиенту, а не обработчику конкретного токена. Поэтому я решил помещать эти данные в структуру `ClientData`, экземпляры которой коллективно используются (через `std::shared_ptr`) в таблицах конкретных обработчиков `TokenHandler::clients_`.

Т.к. может существовать *(ошибочно)* несколько клиентов с одинаковыми **id** на одном или нескольких ip-адресах, я решил, что каждый клиент будет определяется идентификатором **id**, ip адресом и портом. Эти данные сведены в структуру `ClientDescr`. Чтобы использовать эту структуру в качестве ключа ассоциативного массива `TokenHandler::clients_` и не заморачиваться с доп.реализацией, структура сделана на базе `std::tuple`.

Таблицы `TokenHandler::clients_` в обработчиках и сам контейнер обработчиков `TokenHandlersManager::handlers_` представляют собой thread-safe ассоциативные контейнеры `ThreadSafeMap` на базе `std::map`. Я не использовал `std::unordered_map` из соображений того, что данные (новые клиенты и токены) будут добавляться интенсивно, что в случае с `std::unordered_map` привело бы к частому рехешу таблиц.

Для оптимизации работы контейнера в методе `ThreadSafeMap::get_or_create` используется `std::shared_lock` для поиска существующих в контейнере элементов и `std::scoped_lock` для вставки новых.

За периодический вывод содержимого таблицы обработчиков отвечает класс `PeriodicOutput`, который периодически обходит содержимое обработчиков с помощью метода `TokenHandlersManager::for_each_handler`. Период обхода задается в секундах в парамерах запуска сервера. Таблица выводится только в случае, если в ней произошли изменения. Для проверки наличия изменений существует метод `TokenHandlersManager::is_modified`.

Для эмуляции операционной нагрузки обработчиков существует глобальный экземпляр класса `DelaySimulation`. Параметр задержки задается в миллисекундах в парамерах запуска сервера.

Сервер работает в многопоточном режиме. Количество потоков может быть задано при запуске сервера.

## Requirements
* C++ compiler (defaults to gcc)
* [CMake](https://cmake.org/)
* [Boost](http://www.boost.org/)
```
$ sudo apt-get install libboost-all-dev
```

# Usage
Параметры запуска сервера:
```
Usage: server [options]
Options:
  -h [ --help ]         Show help
  -t [ --threads ] arg  running threads 1..[8]
  -s [ --sec ] arg      output period in seconds [10]
  -a [ --addr ] arg     server address [localhost]
  -p [ --port ] arg     server port [9000]
  -d [ --delay ] arg    handler delay simulation in msec +/-10% [0]
```
Параметры запуска клиента:
```
Usage: client [options] <client_id> <filepath>
Options:
  -h [ --help ]         Show help
  --id arg              client ID
  -f [ --file ] arg     tokens file path
  -n [ --num ] arg      number of tokens to send [all]
  -a [ --addr ] arg     server address [localhost]
  -p [ --port ] arg     server port [9000]
```
