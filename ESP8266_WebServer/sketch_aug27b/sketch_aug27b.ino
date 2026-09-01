#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


/* ==================================================
   WiFi
   ================================================== */

const char* ssid = "wifi名称";
const char* password = "wifi密码";


/* ==================================================
   Web服务器
   ================================================== */

ESP8266WebServer server(80);


/* ==================================================
   STM32传过来的数据
   ================================================== */

float temperature = 0.0;

int turbidity = 0;

int pump = 0;

int mode = 0;


/* ==================================================
   浊度阈值

   浊度数值越小 = 水越浑浊

   小于1500 → 水质浑浊
   ================================================== */

const int TURBIDITY_LIMIT = 1500;


/* ==================================================
   串口接收缓存
   ================================================== */

String receiveData = "";


/* ==================================================
   解析STM32数据

   STM32发送：

   T=26.5,TU=1200,P=1,M=1
   ================================================== */

void parseData(String data)
{
    int tIndex = data.indexOf("T=");

    int tuIndex = data.indexOf("TU=");

    int pIndex = data.indexOf("P=");

    int mIndex = data.indexOf("M=");


    /* 如果数据不完整，直接返回 */

    if (tIndex < 0 ||
        tuIndex < 0 ||
        pIndex < 0 ||
        mIndex < 0)
    {
        return;
    }


    /* 找逗号位置 */

    int tEnd = data.indexOf(',', tIndex);

    int tuEnd = data.indexOf(',', tuIndex);

    int pEnd = data.indexOf(',', pIndex);


    if (tEnd < 0 ||
        tuEnd < 0 ||
        pEnd < 0)
    {
        return;
    }


    /* =========================
       温度
       ========================= */

    String tStr =
        data.substring(
            tIndex + 2,
            tEnd
        );


    /* =========================
       浊度
       ========================= */

    String tuStr =
        data.substring(
            tuIndex + 3,
            tuEnd
        );


    /* =========================
       水泵状态
       ========================= */

    String pStr =
        data.substring(
            pIndex + 2,
            pEnd
        );


    /* =========================
       模式
       ========================= */

    String mStr =
        data.substring(
            mIndex + 2
        );


    /* =========================
       转换数据
       ========================= */

    temperature = tStr.toFloat();

    turbidity = tuStr.toInt();

    pump = pStr.toInt();

    mode = mStr.toInt();
}


/* ==================================================
   网页主页
   ================================================== */

void handleRoot()
{
    String html = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta charset="UTF-8">

<meta name="viewport"
      content="width=device-width,initial-scale=1.0">

<title>观赏鱼水体监测系统</title>


<style>

/* =========================
   页面
   ========================= */

body
{
    margin:0;

    padding:0;

    font-family:
        Arial,
        "Microsoft YaHei",
        sans-serif;

    background:
        linear-gradient(
            135deg,
            #e0f7fa,
            #f5fbff
        );

    color:#333;
}


/* =========================
   标题
   ========================= */

.header
{
    text-align:center;

    padding:30px 10px 20px;
}


.header h1
{
    margin:0;

    font-size:28px;

    color:#1565c0;
}


.header p
{
    margin-top:10px;

    color:#777;

    font-size:14px;
}


/* =========================
   数据卡片
   ========================= */

.container
{
    max-width:1000px;

    margin:auto;

    padding:10px 20px;

    display:grid;

    grid-template-columns:
        repeat(
            auto-fit,
            minmax(250px,1fr)
        );

    gap:20px;
}


.card
{
    background:white;

    border-radius:20px;

    padding:25px;

    text-align:center;

    box-shadow:
        0 8px 25px
        rgba(0,0,0,0.08);
}


.icon
{
    font-size:45px;

    margin-bottom:10px;
}


.card h2
{
    margin:5px 0 15px;

    font-size:20px;

    color:#555;
}


.value
{
    font-size:40px;

    font-weight:bold;

    color:#1565c0;
}


.unit
{
    font-size:18px;

    color:#777;
}


/* =========================
   水泵颜色
   ========================= */

.pump-on
{
    color:#2e7d32;
}


.pump-off
{
    color:#757575;
}


/* =========================
   模式
   ========================= */

.mode-manual
{
    color:#1565c0;
}


.mode-auto
{
    color:#ef6c00;
}


/* =========================
   控制区域
   ========================= */

.control
{
    max-width:900px;

    margin:25px auto;

    padding:25px;

    background:white;

    border-radius:20px;

    text-align:center;

    box-shadow:
        0 5px 20px
        rgba(0,0,0,0.06);
}


.control h2
{
    margin-top:0;

    color:#555;
}


/* =========================
   按钮
   ========================= */

button
{
    margin:8px;

    padding:14px 30px;

    border:none;

    border-radius:12px;

    font-size:17px;

    cursor:pointer;

    color:white;
}


button:active
{
    transform:scale(0.95);
}


.mode-button
{
    background:#1565c0;
}


.auto-button
{
    background:#ef6c00;
}


.on-button
{
    background:#2e7d32;
}


.off-button
{
    background:#d32f2f;
}


/* 禁用按钮 */

button:disabled
{
    background:#bdbdbd;

    cursor:not-allowed;

    transform:none;
}


/* =========================
   水质状态
   ========================= */

.status
{
    max-width:900px;

    margin:25px auto;

    padding:18px;

    border-radius:15px;

    background:white;

    text-align:center;

    box-shadow:
        0 5px 20px
        rgba(0,0,0,0.06);
}


.normal
{
    color:#2e7d32;

    font-weight:bold;
}


.warning
{
    color:#d84315;

    font-weight:bold;
}


/* =========================
   更新时间
   ========================= */

.update
{
    text-align:center;

    margin:20px;

    color:#888;

    font-size:13px;
}


.footer
{
    text-align:center;

    padding:25px;

    color:#999;

    font-size:13px;
}

</style>

</head>


<body>


<!-- =========================
     标题
     ========================= -->

<div class="header">

<h1>🐟 观赏鱼水体监测系统</h1>

<p>
STM32F103C8T6 + ESP8266
</p>

</div>



<!-- =========================
     数据显示
     ========================= -->

<div class="container">


<!-- 温度 -->

<div class="card">

<div class="icon">🌡️</div>

<h2>水温</h2>

<div
    class="value"
    id="temperature">
    --.-
</div>

<div class="unit">
    ℃
</div>

</div>



<!-- 浊度 -->

<div class="card">

<div class="icon">💧</div>

<h2>浊度</h2>

<div
    class="value"
    id="turbidity">
    ----
</div>

<div class="unit">
    ADC
</div>

</div>



<!-- 水泵 -->

<div class="card">

<div class="icon">🚰</div>

<h2>水泵状态</h2>

<div
    class="value pump-off"
    id="pump">
    关闭
</div>

</div>



<!-- 模式 -->

<div class="card">

<div class="icon">⚙️</div>

<h2>控制模式</h2>

<div
    class="value mode-manual"
    id="mode">
    手动
</div>

</div>


</div>



<!-- =========================
     模式控制
     ========================= -->

<div class="control">

<h2>⚙️ 水泵控制模式</h2>


<button
    class="mode-button"
    onclick="manualMode()">

手动模式

</button>


<button
    class="auto-button"
    onclick="autoMode()">

自动模式

</button>

</div>



<!-- =========================
     水泵控制
     ========================= -->

<div
    class="control">

<h2>
🚰 水泵手动控制
</h2>


<button
    id="pumpOnButton"
    class="on-button"
    onclick="pumpOn()">

开启水泵

</button>


<button
    id="pumpOffButton"
    class="off-button"
    onclick="pumpOff()">

关闭水泵

</button>


<p id="controlHint">
当前为手动模式，可以控制水泵
</p>

</div>



<!-- =========================
     水质状态
     ========================= -->

<div class="status">

<div
    id="waterStatus"
    class="normal">

正在等待数据...

</div>

</div>



<!-- 更新时间 -->

<div class="update">

最后更新：

<span id="updateTime">
--
</span>

</div>



<div class="footer">

观赏鱼养殖水体监测与控制系统

</div>



<script>


/* ==================================================
   更新网页数据
   ================================================== */

function updateData()
{

    fetch("/data")

    .then(
        response => response.json()
    )

    .then(
        data =>
        {

            /* =====================
               温度
               ===================== */

            document
                .getElementById("temperature")
                .innerHTML =
                Number(
                    data.temperature
                ).toFixed(1);


            /* =====================
               浊度
               ===================== */

            document
                .getElementById("turbidity")
                .innerHTML =
                data.turbidity;


            /* =====================
               水泵
               ===================== */

            let pumpElement =
                document.getElementById(
                    "pump"
                );


            if(data.pump == 1)
            {

                pumpElement.innerHTML =
                    "开启";

                pumpElement.className =
                    "value pump-on";

            }

            else
            {

                pumpElement.innerHTML =
                    "关闭";

                pumpElement.className =
                    "value pump-off";

            }



            /* =====================
               模式
               ===================== */

            let modeElement =
                document.getElementById(
                    "mode"
                );


            if(data.mode == 1)
            {

                modeElement.innerHTML =
                    "自动";

                modeElement.className =
                    "value mode-auto";


            }

            else
            {

                modeElement.innerHTML =
                    "手动";

                modeElement.className =
                    "value mode-manual";

            }



            /* =====================
               手动按钮状态
               ===================== */

            let onButton =
                document.getElementById(
                    "pumpOnButton"
                );


            let offButton =
                document.getElementById(
                    "pumpOffButton"
                );


            let hint =
                document.getElementById(
                    "controlHint"
                );


            if(data.mode == 1)
            {

                /* 自动模式 */

                onButton.disabled = true;

                offButton.disabled = true;

                hint.innerHTML =
                    "当前为自动模式，水泵由系统自动控制";

            }

            else
            {

                /* 手动模式 */

                onButton.disabled = false;

                offButton.disabled = false;

                hint.innerHTML =
                    "当前为手动模式，可以控制水泵";

            }



            /* =====================
               水质判断

               浊度越小越浑浊
               ===================== */

            let statusElement =
                document.getElementById(
                    "waterStatus"
                );


            if(data.turbidity <
               data.limit)
            {

                statusElement.innerHTML =
                    "⚠️ 水质浑浊，浊度超过设定范围";

                statusElement.className =
                    "warning";

            }

            else
            {

                statusElement.innerHTML =
                    "✅ 水质正常";

                statusElement.className =
                    "normal";

            }



            /* =====================
               更新时间
               ===================== */

            let now =
                new Date();


            document
                .getElementById(
                    "updateTime"
                )
                .innerHTML =
                now.toLocaleTimeString();

        }
    )


    .catch(
        error =>
        {

            document
                .getElementById(
                    "waterStatus"
                )
                .innerHTML =
                "⚠️ 无法获取ESP8266数据";

            document
                .getElementById(
                    "waterStatus"
                )
                .className =
                "warning";

        }
    );

}



/* ==================================================
   手动模式
   ================================================== */

function manualMode()
{

    fetch("/mode/manual")

    .then(
        response => response.text()
    )

    .then(
        data =>
        {
            console.log(data);

            updateData();
        }
    );

}



/* ==================================================
   自动模式
   ================================================== */

function autoMode()
{

    fetch("/mode/auto")

    .then(
        response => response.text()
    )

    .then(
        data =>
        {
            console.log(data);

            updateData();
        }
    );

}



/* ==================================================
   开启水泵
   ================================================== */

function pumpOn()
{

    fetch("/pump/on")

    .then(
        response => response.text()
    )

    .then(
        data =>
        {
            console.log(data);

            updateData();
        }
    );

}



/* ==================================================
   关闭水泵
   ================================================== */

function pumpOff()
{

    fetch("/pump/off")

    .then(
        response => response.text()
    )

    .then(
        data =>
        {
            console.log(data);

            updateData();
        }
    );

}



/* ==================================================
   每秒更新一次
   ================================================== */

setInterval(
    updateData,
    1000
);


/* 页面打开立即更新 */

updateData();


</script>


</body>

</html>

)rawliteral";


    server.send(
        200,
        "text/html",
        html
    );
}



/* ==================================================
   /data

   返回STM32当前数据
   ================================================== */

void handleData()
{

    String json = "{";


    json += "\"temperature\":";

    json +=
        String(
            temperature,
            1
        );


    json += ",";


    json += "\"turbidity\":";

    json +=
        String(
            turbidity
        );


    json += ",";


    json += "\"pump\":";

    json +=
        String(
            pump
        );


    json += ",";


    json += "\"mode\":";

    json +=
        String(
            mode
        );


    json += ",";


    json += "\"limit\":";

    json +=
        String(
            TURBIDITY_LIMIT
        );


    json += "}";


    server.send(
        200,
        "application/json",
        json
    );
}



/* ==================================================
   手动开启水泵
   ================================================== */

void handlePumpOn()
{

    /*
       只有手动模式允许网页控制
    */

    if(mode == 0)
    {

        Serial.print(
            "P=1\r\n"
        );

        pump = 1;

        server.send(
            200,
            "text/plain",
            "PUMP ON"
        );

    }

    else
    {

        server.send(
            200,
            "text/plain",
            "AUTO MODE"
        );

    }

}



/* ==================================================
   手动关闭水泵
   ================================================== */

void handlePumpOff()
{

    /*
       只有手动模式允许网页控制
    */

    if(mode == 0)
    {

        Serial.print(
            "P=0\r\n"
        );

        pump = 0;

        server.send(
            200,
            "text/plain",
            "PUMP OFF"
        );

    }

    else
    {

        server.send(
            200,
            "text/plain",
            "AUTO MODE"
        );

    }

}



/* ==================================================
   切换手动模式
   ================================================== */

void handleManualMode()
{

    Serial.print(
        "MODE=0\r\n"
    );


    mode = 0;


    server.send(
        200,
        "text/plain",
        "MANUAL MODE"
    );
}



/* ==================================================
   切换自动模式
   ================================================== */

void handleAutoMode()
{

    Serial.print(
        "MODE=1\r\n"
    );


    mode = 1;


    server.send(
        200,
        "text/plain",
        "AUTO MODE"
    );
}



/* ==================================================
   ESP8266初始化
   ================================================== */

void setup()
{

    /* =====================
       串口
       ===================== */

    Serial.begin(
        115200
    );


    delay(1000);


    Serial.println();

    Serial.println(
        "ESP8266启动"
    );


    /* =====================
       连接WiFi
       ===================== */

    WiFi.begin(
        ssid,
        password
    );


    Serial.print(
        "正在连接WiFi"
    );


    while(
        WiFi.status() !=
        WL_CONNECTED
    )
    {

        delay(500);

        Serial.print(
            "."
        );

    }


    Serial.println();

    Serial.println(
        "WiFi连接成功"
    );


    Serial.print(
        "IP地址："
    );

    Serial.println(
        WiFi.localIP()
    );


    /* =====================
       注册网页
       ===================== */

    server.on(
        "/",
        handleRoot
    );


    server.on(
        "/data",
        handleData
    );


    server.on(
        "/pump/on",
        handlePumpOn
    );


    server.on(
        "/pump/off",
        handlePumpOff
    );


    server.on(
        "/mode/manual",
        handleManualMode
    );


    server.on(
        "/mode/auto",
        handleAutoMode
    );


    /* =====================
       启动服务器
       ===================== */

    server.begin();


    Serial.println(
        "网页服务器启动"
    );

}



/* ==================================================
   主循环
   ================================================== */

void loop()
{

    /* =====================
       接收STM32数据
       ===================== */

    while(
        Serial.available()
    )
    {

        char c =
            Serial.read();


        if(c == '\n')
        {

            parseData(
                receiveData
            );


            receiveData =
                "";

        }

        else
        {

            if(c != '\r')
            {
                receiveData += c;
            }

        }

    }


    /* =====================
       处理网页请求
       ===================== */

    server.handleClient();

}