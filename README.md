# Shell Time Manager

## 📚 Описание
Примитивная командная оболочка на языке C с поддержкой:
- Регистрации и входа пользователей
- Авторизации по логину и PIN
- Хеширования паролей (libcrypt)
- Статуса администратора (первый зарегистрированный пользователь)
- Команд:
  - `register <login> <pin>` — регистрация
  - `login <login> <pin>` — вход
  - `logout` — выход
  - `time`, `date` — текущее время и дата
  - `howmuch <dd/mm/yyyy> <-s|-m|-h|-y|-d>` — разница с текущей датой
  - `sanctions <login> <number>` — установить лимит команд (только админ)
  - `listusers` — просмотр пользователей (только админ)
  - `help`, `exit`

## 🛠 Сборка и запуск

```bash
make
./app
```

## 🧹 Очистка
```bash
make clean
```

## 📄 База данных
Файл `db.txt` хранит пользователей:
```
login hashed_password attempts
```
Пример:
```
admin $6$.... 5
```

## 📌 Примечания
- Первый зарегистрированный пользователь становится админом
- Все команды проверяют авторизацию
- Админ имеет неограниченные попытки
















# Полное объяснение программы

## 📦 Назначение
Это учебная командная оболочка (`shell`) на языке C, реализующая:
- регистрацию и вход пользователей
- хеширование PIN-кодов
- авторизацию (админ/обычный)
- выполнение команд (`time`, `date`, `howmuch`)
- санкции и список пользователей (только для админа)

---

## 📁 Структура проекта

| Файл            | Назначение |
|-----------------|------------|
| `main.c`        | Основной ввод и логика |
| `db.c/.h`       | Регистрация, вход, PIN, база |
| `user.h`        | Структура пользователя |
| `enums.h`       | Коды возврата (`SUCCESS`, `ERROR`, ...) |
| `date_utils.c/.h`| Работа с датами |
| `db.txt`        | База данных |
| `Makefile`      | Сборка |
| `README.md`     | Описание проекта |
| `code_explained.md` | Это файл: разбор программы |

---

## 🔄 Поток выполнения

1. Запуск `./app` (или `make run`)
2. Вывод приветствия
3. `while (1)` — цикл ввода
4. Разбор команд
5. Проверка состояния (вход, админ)
6. Выполнение: `time`, `date`, `howmuch`, `sanctions`, `listusers`, ...
7. `exit` — завершает работу

---

## 🔧 Основные функции

### `register_user(...)`
- Проверяет логин и PIN
- Хеширует PIN
- Записывает в `db.txt`: `login hash attempts`

### `login_user(...)`
- Ищет логин
- Сравнивает PIN (соль+хеш)
- Устанавливает флаги `user.is_logged_in`, `user.is_admin`

### `list_users(...)`
- Вывод всех пользователей

### `change_attempts(...)`
- Помечает старую строку `NU`
- Добавляет новую с лимитом

---

## 📌 Встроенные функции

| Функция        | Назначение |
|----------------|------------|
| `fgets()`      | Ввод строки с консоли или файла |
| `strtok()`     | Разбиение строки на части |
| `fopen()`      | Открытие файла |
| `fprintf()`    | Запись в файл |
| `crypt_ra()`   | Хеширование PIN с солью |
| `crypt_gensalt_ra()` | Генерация соли |
| `strdup()`     | Копирование строки |
| `atoi()`       | Строка → число |
| `sscanf()`     | Разбор строки по формату |

---

## 🧠 Примеры использования

```
register admin 1234
login admin 1234
time
date
howmuch 01/01/2000 -y
sanctions user1 3
listusers
logout
exit
```

---

## 🧪 Админ
- Первый пользователь в `db.txt` — это админ
- Админ не имеет ограничений по попыткам
- Только админ может вызывать `sanctions` и `listusers`

---

## 🧹 Очистка и сборка

```bash
make clean
make
make run
```

---

