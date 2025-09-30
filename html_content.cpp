#include <Arduino.h>




//---------------------------------------------------------Страница диалога с энтузиастом---------------
const String HTML_DIALOGUE_1 PROGMEM = R"rawliteral(
  <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Chat</title>
    <link rel="stylesheet" href="styles.css">
</head>
<body>
    <div class="scanline"></div>
    
    <div class="container">
        <div class="cyber-navbar">
            <form method="post" action="/messenger">
                <button type="submit" class="cyber-button">←</button>
            </form>
            <div class="name neon-text">)rawliteral";

const String HTML_DIALOGUE_2 PROGMEM = R"rawliteral(</div>
          <div class="logo neon-text">///ME-LO-RA///</div>
        </div>
        
        <!-- Область сообщений -->
        <div class="messages-container" id="messages">
            <!-- Сообщения будут добавляться здесь -->)rawliteral";

const String HTML_DIALOGUE_3 PROGMEM = R"rawliteral(</div>
    <div class="input-area" style="padding:20px;">
            <form method="post" action="/dialogue">
                <div style="display:flex;gap:10px;">
                    <input 
                        name="text"
                        type="text" 
                        class="cyber-input" 
                        id="messageInput" 
                        placeholder="ВВЕДИТЕ СООБЩЕНИЕ..."
                        style="flex:1;"
                    >
                    <input type="hidden" name="dialog_id" value=")rawliteral";

const String HTML_DIALOGUE_4 PROGMEM = R"rawliteral(">
                    <input type="hidden" name="sended" value="1">
                    <button type="submit" class="cyber-button">➤</button>
                </div>
            </form>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            const messagesContainer = document.getElementById('messages');
            messagesContainer.scrollTop = messagesContainer.scrollHeight;
            
            const messageForm = document.querySelector('.input-area form');
            const messageInput = document.getElementById('messageInput');
            
            // Add glitch effect to all neon text periodically
            setInterval(() => {
                document.querySelectorAll('.neon-text').forEach(el => {
                    el.style.animation = 'none';
                    setTimeout(() => {
                        el.style.animation = 'glitch 12s infinite';
                    }, 10);
                });
            }, 15000);
        });
    </script>
</body>
</html>)rawliteral";





//------------------------------------------------Страница обработки данных регистрации пользователя и добавления энтузиастов----------------
const String HTML_OBR_1 PROGMEM = R"rawliteral(
                <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Panel</title>
    <link rel="stylesheet" href="styles.css">
</head>
<body>
    <div class="scanline"></div>
    
    <div class="container">
        <div class="cyber-header">
            <h1 class="neon-text">///ME-LO-RA///</h1>
            <h2 class="neon-pink">ДОБАВЛЕНИЕ ЭНТУЗИАСТОВ</h2>
        </div>
        
        <div class="form-container">
            <div class="cyber-panel pink-glow">
                <form name="testl" method="post" action="/add">
                    <div class="form-row">
                        <p>ПСЕВДОНИМ:</p>
                        <input class="cyber-input" name="l_name" pattern="^[a-zA-Z\s]+$" value="SOMEBODY">
                    </div>
                    
                    <div class="form-row">
                        <p>ОТКРЫТАЯ ЭКСПОНЕНТА (e):</p>
                        <input class="cyber-input" name="ue_exp" pattern="^[ 0-9]+$" value="706571603">
                    </div>
                    
                    <div class="form-row">
                        <p>МОДУЛЬ (n):</p>
                        <input class="cyber-input" name="umodule" pattern="^[ 0-9]+$" value="541432178183809597">
                    </div>
                    
                    <div class="text-center grid-3">
                        <button type="submit" class="cyber-button">ДОБАВИТЬ</button>
                        <a href="/messenger" class="cyber-button pink">ВОЙТИ В СЕТЬ</a>
                    </div>
                </form>
            </div>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            // Add glitch effect to all neon text periodically
            setInterval(() => {
                document.querySelectorAll('.neon-text, .neon-pink').forEach(el => {
                    el.style.animation = 'none';
                    setTimeout(() => {
                        el.style.animation = 'glitch 12s infinite';
                    }, 10);
                });
            }, 15000);
            
            // Form input focus effect
            document.querySelectorAll('.cyber-input').forEach(input => {
                input.addEventListener('focus', function() {
                    this.style.boxShadow = '0 0 20px var(--neon-pink)';
                });
                
                input.addEventListener('blur', function() {
                    this.style.boxShadow = '';
                });
            });
        });
    </script>
</body>
</html>)rawliteral";



//-------------------------------------------------Добавление энтузиастов-----------------
const String HTML_LOS_1 PROGMEM = R"rawliteral(
                <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Panel</title>
    <link rel="stylesheet" href="styles.css">
</head>
<body>
    <div class="scanline"></div>
    
    <div class="container">
        <div class="cyber-header">
            <h1 class="neon-text">///ME-LO-RA///</h1>
            <h2 class="neon-pink">ДОБАВЛЕНИЕ ЭНТУЗИАСТОВ</h2>
        </div>
        
        <div class="form-container">
            <div class="cyber-panel pink-glow">
                <form name="testl" method="post" action="/add">
                    <div class="form-row">
                        <p>ПСЕВДОНИМ:</p>
                        <input class="cyber-input" name="l_name" pattern="^[a-zA-Z\s]+$" value="SOMEBODY">
                    </div>
                    
                    <div class="form-row">
                        <p>ОТКРЫТАЯ ЭКСПОНЕНТА (e):</p>
                        <input class="cyber-input" name="ue_exp" pattern="^[ 0-9]+$" value="706571603">
                    </div>
                    
                    <div class="form-row">
                        <p>МОДУЛЬ (n):</p>
                        <input class="cyber-input" name="umodule" pattern="^[ 0-9]+$" value="541432178183809597">
                    </div>
                    
                    <div class="text-center grid-3">
                        <button type="submit" class="cyber-button">ДОБАВИТЬ</button>
                        <a href="/messenger" class="cyber-button pink">ВОЙТИ В СЕТЬ</a>
                    </div>
                </form>
            </div>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            // Add glitch effect to all neon text periodically
            setInterval(() => {
                document.querySelectorAll('.neon-text, .neon-pink').forEach(el => {
                    el.style.animation = 'none';
                    setTimeout(() => {
                        el.style.animation = 'glitch 12s infinite';
                    }, 10);
                });
            }, 15000);
            
            // Form input focus effect
            document.querySelectorAll('.cyber-input').forEach(input => {
                input.addEventListener('focus', function() {
                    this.style.boxShadow = '0 0 20px var(--neon-pink)';
                });
                
                input.addEventListener('blur', function() {
                    this.style.boxShadow = '';
                });
            });
        });
    </script>
</body>
</html>)rawliteral";





//--------------------------------------------------Главная страница с диалогами------------------------------
const String HTML_MESSENGER_1 PROGMEM = R"rawliteral(
              <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Network</title>
    <link rel="stylesheet" href="styles.css">
</head>
<body>
    <div class="scanline"></div>
    <div class="container">
        <div class="cyber-header">
            <h1 class="neon-text">///ME-LO-RA///</h1>
            <h2 class="neon-pink">СЕТЬ</h2>
        </div>
        
        <main>
            <div class="dialogs-container">
  )rawliteral";
  
const String HTML_MESSENGER_2 PROGMEM = R"rawliteral(
                      </div>
                    </main>
                    <!-- Нижнее меню с кнопками -->
                    <div class="cyber-footer">
            <form method="post" action="/add">
                <button type="submit" class="cyber-button">ДОБАВИТЬ ЭНТУЗИАСТОВ</button>
            </form>
            
            <form method="post" action="/">
                <button type="submit" class="cyber-button pink">РАЗЛОГИНИТЬСЯ</button>
            </form>
            
            <form method="post" action="/settings">
                <button type="submit" class="cyber-button">НАСТРОЙКИ</button>
            </form>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            // Add glitch effect to all neon text periodically
            setInterval(() => {
                document.querySelectorAll('.neon-text, .neon-pink').forEach(el => {
                    el.style.animation = 'none';
                    setTimeout(() => {
                        el.style.animation = 'glitch 12s infinite';
                    }, 10);
                });
            }, 15000);
            
            // Dialog item click effect
            document.querySelectorAll('.dialog-item').forEach(item => {
                item.addEventListener('click', function() {
                    this.style.boxShadow = '0 0 30px var(--neon-pink)';
                    this.style.transform = 'scale(1.02)';
                    setTimeout(() => {
                        this.style.boxShadow = '';
                        this.style.transform = '';
                    }, 500);
                });
            });
        });
    </script>
</body>
</html>)rawliteral";

//--------------------------------------------------------SETTINGS---------------------------------
const String HTML_SETTINGS_1 PROGMEM = R"rawliteral(
  <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Настройки</title>
    <link rel="stylesheet" href="styles.css">
    <style>
        /* Дополнительные стили для страницы настроек */
        .stats-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .stat-item {
            background: linear-gradient(to right, var(--darker-bg), rgba(15, 25, 50, 0.7));
            border: 1px solid var(--neon-blue);
            padding: 15px;
            border-radius: 4px;
            position: relative;
            overflow: hidden;
            box-shadow: var(--panel-glow);
        }
        
        .stat-item::before {
            content: "";
            position: absolute;
            top: 0;
            left: 0;
            width: 5px;
            height: 100%;
            background: linear-gradient(to bottom, var(--neon-blue), var(--neon-purple));
        }
        
        .stat-label {
            color: var(--neon-blue);
            text-shadow: var(--text-glow);
            font-size: 0.9rem;
            margin-bottom: 5px;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        .stat-value {
            color: #fff;
            font-size: 1.4rem;
            text-shadow: var(--text-glow);
        }
        
        .data-section {
            margin-top: 30px;
        }
        
        .data-toggle {
            background: linear-gradient(45deg, var(--darker-bg), #0f1a2a);
            border: 1px solid var(--neon-pink);
            color: var(--neon-pink);
            padding: 12px 24px;
            font-size: 16px;
            cursor: pointer;
            text-shadow: var(--text-pink-glow);
            box-shadow: var(--panel-pink-glow);
            width: 100%;
            text-align: left;
            position: relative;
            transition: all 0.3s ease;
            margin-bottom: 10px;
        }
        
        .data-toggle::after {
            content: "▼";
            position: absolute;
            right: 15px;
            transition: transform 0.3s ease;
        }
        
        .data-toggle.active::after {
            transform: rotate(180deg);
        }
        
        .data-content {
            display: none;
            background-color: rgba(5, 10, 20, 0.7);
            border: 1px solid var(--neon-purple);
            padding: 15px;
            border-radius: 4px;
            box-shadow: inset 0 0 20px rgba(160, 0, 255, 0.3);
            max-height: 300px;
            overflow-y: auto;
        }
        
        .data-content.active {
            display: block;
            animation: fadeIn 0.5s ease;
        }
        
        .data-text {
            color: #a0a0ff;
            font-family: 'Courier New', monospace;
            line-height: 1.5;
            white-space: pre-wrap;
        }
        
        .progress-bar {
            height: 10px;
            background-color: rgba(5, 10, 20, 0.7);
            border-radius: 5px;
            margin-top: 10px;
            overflow: hidden;
            position: relative;
        }
        
        .progress-fill {
            height: 100%;
            background: linear-gradient(to right, var(--neon-blue), var(--neon-purple));
            border-radius: 5px;
            width: 65%; /* Значение для примера */
            position: relative;
        }
        
        .progress-fill::after {
            content: "";
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(90deg, 
                transparent, 
                rgba(255, 255, 255, 0.4), 
                transparent);
            animation: progressShine 2s infinite;
        }
        
        @keyframes progressShine {
            0% { transform: translateX(-100%); }
            100% { transform: translateX(100%); }
        }
        
        /* Анимация изменения значений */
        @keyframes valueChange {
            0% { color: #fff; text-shadow: var(--text-glow); }
            50% { color: var(--neon-pink); text-shadow: var(--text-pink-glow); }
            100% { color: #fff; text-shadow: var(--text-glow); }
        }
        
        .value-updated {
            animation: valueChange 1s ease;
        }
    </style>
</head>
<body>
    <div class="scanline"></div>
    <div class="container">
        <div class="cyber-header">
            <h1 class="neon-text">///ME-LO-RA///</h1>
            <h2 class="neon-pink">СИСТЕМНЫЕ НАСТРОЙКИ</h2>
        </div>)rawliteral";

const String HTML_SETTINGS_2 PROGMEM = R"rawliteral(<div style="text-align: center; margin-top: 30px;">
            <form method="post" action="/messenger">
                <button type="submit" class="cyber-button">ВЕРНУТЬСЯ</button>
            </form>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            // Добавление эффекта глитча для неонового текста
            setInterval(() => {
                document.querySelectorAll('.neon-text, .neon-pink').forEach(el => {
                    el.style.animation = 'none';
                    setTimeout(() => {
                        el.style.animation = 'glitch 12s infinite';
                    }, 10);
                });
            }, 15000);
            
            // Переключение отображения данных
            const dataToggle = document.getElementById('data-toggle');
            const dataContent = document.getElementById('data-content');
            
            dataToggle.addEventListener('click', function() {
                this.classList.toggle('active');
                dataContent.classList.toggle('active');
            });
        });
    </script>
</body>
</html>)rawliteral";



//-----------------------------------------------------------------CSS FILE------------------------------------------------------
const String HTML_CSS_1 PROGMEM = R"rawliteral(
:root {
    --neon-blue: #00f3ff;
    --neon-pink: #ff00ea;
    --neon-purple: #a600ff;
    --dark-bg: #0a0a14;
    --darker-bg: #050510;
    --panel-bg: rgba(10, 15, 30, 0.8);
    --text-glow: 0 0 8px var(--neon-blue);
    --text-pink-glow: 0 0 8px var(--neon-pink);
    --panel-glow: 0 0 15px rgba(0, 243, 255, 0.7);
    --panel-pink-glow: 0 0 15px rgba(255, 0, 234, 0.5);
}

* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
    font-family: 'Courier New', monospace;
}

body {
    background-color: var(--dark-bg);
    color: #e0e0ff;
    background-image: 
        linear-gradient(rgba(0, 243, 255, 0.05) 1px, transparent 1px),
        linear-gradient(90deg, rgba(0, 243, 255, 0.05) 1px, transparent 1px);
    background-size: 20px 20px;
    min-height: 100vh;
    overflow-x: hidden;
    position: relative;
}

body::before {
    content: "";
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: repeating-linear-gradient(
        0deg,
        rgba(0, 0, 0, 0.15),
        rgba(0, 0, 0, 0.15) 1px,
        transparent 1px,
        transparent 2px
    );
    pointer-events: none;
    z-index: 1000;
}

/* SCANLINE ANIMATION */
@keyframes scanline {
    0% { transform: translateY(-100%); }
    100% { transform: translateY(100vh); }
}

.scanline {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: linear-gradient(
        to bottom,
        rgba(0, 243, 255, 0) 0%,
        rgba(0, 243, 255, 0.2) 10%,
        rgba(0, 243, 255, 0) 100%
    );
    animation: scanline 8s linear infinite;
    pointer-events: none;
    z-index: 1001;
}

/* GLITCH EFFECT */
@keyframes glitch {
    0% { text-shadow: var(--text-glow); }
    2% { text-shadow: 0.1em 0 0 var(--neon-pink), -0.1em -0.05em 0 var(--neon-blue); }
    4% { text-shadow: var(--text-glow); }
    96% { text-shadow: var(--text-glow); }
    98% { text-shadow: -0.1em 0 0 var(--neon-pink), 0.1em 0.05em 0 var(--neon-blue); }
    100% { text-shadow: var(--text-glow); }
}

/* NEON GLOW */
.neon-text {
    text-shadow: var(--text-glow);
    animation: glitch 12s infinite;
    color: #fff;
}

.neon-pink {
    text-shadow: var(--text-pink-glow);
    color: #ffb0f7;
}

/* PANELS AND CONTAINERS */
.cyber-panel {
    background-color: var(--panel-bg);
    border: 1px solid var(--neon-blue);
    box-shadow: var(--panel-glow), inset var(--panel-glow);
    padding: 20px;
    border-radius: 4px;
    margin-bottom: 20px;
    position: relative;
    overflow: hidden;
}

.cyber-panel::before {
    content: "";
    position: absolute;
    top: 0;
    left: 0;
    width: 5px;
    height: 100%;
    background: linear-gradient(to bottom, var(--neon-blue), var(--neon-purple));
    box-shadow: 0 0 10px var(--neon-blue);
}

.cyber-panel.pink-glow {
    border-color: var(--neon-pink);
    box-shadow: var(--panel-pink-glow), inset var(--panel-pink-glow);
}

.cyber-panel.pink-glow::before {
    background: linear-gradient(to bottom, var(--neon-pink), var(--neon-purple));
    box-shadow: 0 0 10px var(--neon-pink);
}

/* BUTTONS */
.cyber-button {
    background: linear-gradient(45deg, var(--darker-bg), #0f1a2a);
    border: 1px solid var(--neon-blue);
    color: var(--neon-blue);
    padding: 12px 24px;
    font-size: 16px;
    cursor: pointer;
    position: relative;
    overflow: hidden;
    text-shadow: var(--text-glow);
    box-shadow: var(--panel-glow);
    transition: all 0.3s ease;
    outline: none;
    border-radius: 2px;
    text-transform: uppercase;
    letter-spacing: 2px;
}

.cyber-button:hover {
    background: linear-gradient(45deg, #0f1a2a, var(--darker-bg));
    box-shadow: 0 0 20px var(--neon-blue);
    text-shadow: 0 0 15px var(--neon-blue);
    transform: translateY(-2px);
}

.cyber-button:active {
    transform: translateY(1px);
}

.cyber-button.pink {
    border-color: var(--neon-pink);
    color: var(--neon-pink);
    text-shadow: var(--text-pink-glow);
    box-shadow: var(--panel-pink-glow);
}

.cyber-button.pink:hover {
    box-shadow: 0 0 20px var(--neon-pink);
    text-shadow: 0 0 15px var(--neon-pink);
}

.cyber-button::before {
    content: "";
    position: absolute;
    top: -10px;
    left: -10px;
    right: -10px;
    bottom: -10px;
    background: linear-gradient(45deg, 
        var(--neon-blue), 
        transparent, 
        transparent, 
        var(--neon-blue));
    z-index: -1;
    transform: rotate(45deg) scale(0);
    transition: transform 0.5s ease;
}

.cyber-button:hover::before {
    transform: rotate(45deg) scale(1.5);
}

.cyber-button.pink::before {
    background: linear-gradient(45deg, 
        var(--neon-pink), 
        transparent, 
        transparent, 
        var(--neon-pink));
}

/* INPUTS */
.cyber-input {
    background-color: rgba(5, 10, 20, 0.7);
    border: 1px solid var(--neon-blue);
    color: var(--neon-blue);
    padding: 12px 15px;
    font-size: 16px;
    width: 100%;
    margin-bottom: 15px;
    box-shadow: inset 0 0 10px rgba(0, 243, 255, 0.3), 0 0 10px rgba(0, 243, 255, 0.2);
    transition: all 0.3s ease;
    outline: none;
    text-shadow: var(--text-glow);
}

.cyber-input:focus {
    border-color: var(--neon-pink);
    box-shadow: inset 0 0 15px rgba(255, 0, 234, 0.5), 0 0 15px rgba(255, 0, 234, 0.4);
}

/* HEADER STYLES */
.cyber-header {
    background-color: var(--darker-bg);
    padding: 20px;
    text-align: center;
    border-bottom: 2px solid var(--neon-blue);
    box-shadow: 0 0 20px rgba(0, 243, 255, 0.5);
    margin-bottom: 30px;
    position: relative;
    z-index: 10;
}

.cyber-header h1 {
    font-size: 2.5rem;
    letter-spacing: 4px;
    margin-bottom: 10px;
}

.cyber-header h2 {
    font-size: 1.5rem;
    color: var(--neon-pink);
    text-shadow: var(--text-pink-glow);
}

/* NAVBAR STYLES */
.cyber-navbar {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 15px 20px;
    background-color: var(--darker-bg);
    border-bottom: 1px solid var(--neon-blue);
    box-shadow: 0 5px 15px rgba(0, 243, 255, 0.3);
    position: relative;
}

.cyber-navbar .logo {
    font-size: 1.8rem;
    letter-spacing: 3px;
    position: absolute;
    left: 50%;
    transform: translateX(-50%);
}

/* MESSAGES STYLES */
.messages-container {
    max-height: 60vh;
    overflow-y: auto;
    padding: 20px;
    background-color: rgba(5, 10, 20, 0.5);
    border: 1px solid var(--neon-purple);
    margin-bottom: 20px;
    box-shadow: inset 0 0 20px rgba(160, 0, 255, 0.3);
}

.message {
    padding: 12px 20px;
    margin-bottom: 15px;
    border-radius: 4px;
    max-width: 80%;
    position: relative;
    animation: fadeIn 0.5s ease;
    border: 1px solid transparent;
}

@keyframes fadeIn {
    from { opacity: 0; transform: translateY(10px); }
    to { opacity: 1; transform: translateY(0); }
}

.message.received {
    background: linear-gradient(to right, rgba(10, 25, 50, 0.8), rgba(5, 15, 40, 0.8));
    border-color: var(--neon-blue);
    box-shadow: 0 0 10px rgba(0, 243, 255, 0.3);
    align-self: flex-start;
    border-left: 3px solid var(--neon-blue);
}

.message.sent {
    background: linear-gradient(to left, rgba(50, 10, 50, 0.8), rgba(40, 5, 40, 0.8));
    border-color: var(--neon-pink);
    box-shadow: 0 0 10px rgba(255, 0, 234, 0.3);
    margin-left: auto;
    border-right: 3px solid var(--neon-pink);
}

.message::after {
    content: "";
    position: absolute;
    bottom: 5px;
    font-size: 0.7rem;
    color: #aaa;
}

/* DIALOG LIST */
.dialogs-container {
    display: grid;
    gap: 15px;
    padding: 20px;
}

.dialog-item {
    background: linear-gradient(to right, var(--darker-bg), rgba(15, 25, 50, 0.7));
    border: 1px solid var(--neon-blue);
    padding: 15px;
    border-radius: 4px;
    cursor: pointer;
    transition: all 0.3s ease;
    position: relative;
    overflow: hidden;
}

.dialog-item:hover {
    transform: translateX(5px);
    box-shadow: 0 0 20px rgba(0, 243, 255, 0.5);
    border-color: var(--neon-pink);
}

.dialog-name {
    font-size: 1.2rem;
    color: var(--neon-blue);
    margin-bottom: 5px;
    text-shadow: var(--text-glow);
}

.dialog-message {
    font-size: 0.9rem;
    color: #a0a0ff;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

/* FOOTER STYLES */
.cyber-footer {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
    gap: 15px;
    padding: 20px;
    background-color: var(--darker-bg);
    border-top: 1px solid var(--neon-blue);
    box-shadow: 0 -5px 15px rgba(0, 243, 255, 0.3);
    position: fixed;
    bottom: 0;
    width: 100%;
}

/* FORM LAYOUT */
.form-container {
    max-width: 600px;
    margin: 30px auto;
    padding: 20px;
}

.form-row {
    margin-bottom: 20px;
}

.form-row p {
    margin-bottom: 8px;
    color: var(--neon-blue);
    text-shadow: var(--text-glow);
}

/* UTILITY CLASSES */
.text-center {
    text-align: center;
}

.mb-20 {
    margin-bottom: 20px;
}

.mt-20 {
    margin-top: 20px;
}

.grid-3 {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 15px;
}

/* RESPONSIVE */
@media (max-width: 768px) {
    .grid-3 {
        grid-template-columns: 1fr;
    }
    
    .cyber-navbar .logo {
        font-size: 1.4rem;
    }
    
    .cyber-header h1 {
        font-size: 2rem;
    }
    
    .cyber-footer {
        grid-template-columns: 1fr;
        position: static;
    }
}
  )rawliteral";





//---------------------------------------------------------------Регистрация пользователя--------------------------------------
const String HTML_REG_1 PROGMEM = R"rawliteral(
              <!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ME-LO-RA Registration</title>
    <link rel="stylesheet" href="styles.css">
    <style>
        /* Terminal boot screen styles */
        #boot-screen {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-color: #000;
            z-index: 2000;
            display: flex;
            align-items: center;
            justify-content: center;
            font-family: 'Courier New', monospace;
        }
        
        #boot-content {
            color: #0f0;
            font-size: 18px;
            padding: 20px;
            max-width: 800px;
        }
        
        .terminal-text {
            display: inline-block;
            overflow: hidden;
            white-space: nowrap;
            border-right: 2px solid #0f0;
            animation: 
                terminalTyping 3s steps(40, end),
                blinkCursor 0.75s step-end infinite;
        }
        
        @keyframes terminalTyping {
            from { width: 0; }
            to { width: 100%; }
        }
        
        @keyframes blinkCursor {
            from, to { border-color: transparent; }
            50% { border-color: #0f0; }
        }
    </style>
</head>
<body>
    <div class="scanline"></div>
    
    <!-- Terminal Boot Animation -->
    <div id="boot-screen">
        <div id="boot-content">
            <div>> Booting ME-LO-RA Network OS...</div>
            <div>> Initializing cyber modules <span id="loading-dots">...</span></div>
            <div>> Establishing neural connection</div>
            <div>> <span class="terminal-text">Access granted. Welcome to the network.</span></div>
        </div>
    </div>
    
    <div class="container" style="display:none;" id="content">
        <div class="cyber-header">
            <h1 class="neon-text">///ME-LO-RA///</h1>
            <h2 class="neon-pink">УСТАНОВОЧНЫЕ НАСТРОЙКИ</h2>
        </div>
        
        <div class="form-container">
            <div class="cyber-panel pink-glow">
                <form name="test" method="post" action="/registration">
                    <div class="form-row">
                        <p>ВВЕДИТЕ ПСЕВДОНИМ:</p>
                        <input class="cyber-input" name="user_name" pattern="^[a-zA-Z\s]+$" value="USERNAME">
                    </div>
                    
                    <div class="form-row">
                        <p>ВВЕДИТЕ ОТКРЫТУЮ ЭКСПОНЕНТУ (e):</p>
                        <input class="cyber-input" name="e_exp" pattern="^[ 0-9]+$" value="0">
                    </div>
                    
                    <div class="form-row">
                        <p>ВВЕДИТЕ МОДУЛЬ (n):</p>
                        <input class="cyber-input" name="module" pattern="^[ 0-9]+$" value="0">
                    </div>
                    
                    <div class="form-row">
                        <p>ВВЕДИТЕ ЗАКРЫТУЮ ЭКСПОНЕНТУ (d):</p>
                        <input class="cyber-input" name="d_exp" pattern="^[ 0-9]+$" value="0">
                    </div>
                    
                    <div class="text-center grid-3">
                        <button type="submit" class="cyber-button">ДАЛЕЕ</button>
                        <button type="reset" class="cyber-button pink">ОЧИСТИТЬ</button>
                    </div>
                </form>
            </div>
        </div>
    </div>
    
    <script>
        document.addEventListener('DOMContentLoaded', function() {
            const bootScreen = document.getElementById('boot-screen');
            const content = document.getElementById('content');
            const loadingDots = document.getElementById('loading-dots');
            
            // Animate loading dots
            let dots = 0;
            const dotsInterval = setInterval(() => {
                dots = (dots + 1) % 4;
                loadingDots.textContent = '.'.repeat(dots);
            }, 500);
            
            // Simulate boot process
            setTimeout(() => {
                clearInterval(dotsInterval);
                bootScreen.style.opacity = '0';
                
                setTimeout(() => {
                    bootScreen.style.display = 'none';
                    content.style.display = 'block';
                    
                    // Add glitch effect to all neon text periodically
                    setInterval(() => {
                        document.querySelectorAll('.neon-text, .neon-pink').forEach(el => {
                            el.style.animation = 'none';
                            setTimeout(() => {
                                el.style.animation = 'glitch 12s infinite';
                            }, 10);
                        });
                    }, 15000);
                }, 1000);
            }, 5000);
            
            // Form input focus effect
            document.querySelectorAll('.cyber-input').forEach(input => {
                input.addEventListener('focus', function() {
                    this.style.boxShadow = '0 0 20px var(--neon-pink)';
                });
                
                input.addEventListener('blur', function() {
                    this.style.boxShadow = '';
                });
            });
        });
    </script>
</body>
</html>
  )rawliteral";
