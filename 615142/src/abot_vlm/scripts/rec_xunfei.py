#!/usr/bin/env python3
import pyaudio
import time
import base64
import hmac
import hashlib
import time
import urllib.parse
import requests
import json

# 替换为你的讯飞星火 AppID、API Key 和 API Secret
APPID = '31ec8377'
API_SECRET = 'OThlMTc4MjFmYmNjNjk4MDNmZmJjYzU5'
API_KEY = '9691e4a0ba827280089787f959321757'

def generate_signature(host, date, api_key, api_secret):
    algorithm = 'hmac-sha256'
    headers = f'host date request-line'
    signature_headers = f'host: {host}\ndate: {date}\nPOST /v2/iat HTTP/1.1'
    signing_string = f'{algorithm}\n{date}\n{headers}\n{hashlib.sha256(signature_headers.encode()).hexdigest()}'
    signature = base64.b64encode(
        hmac.new(API_SECRET.encode(), signing_string.encode(), digestmod=hashlib.sha256).digest()).decode()
    authorization = f'api_key="{api_key}", algorithm="{algorithm}", headers="{headers}", signature="{signature}"'
    return authorization

def SaveWave():
    # 设置音频参数
    FORMAT = pyaudio.paInt16
    RATE = 16000
    CHUNK = 4000
    print("开始录音...")
    audio = pyaudio.PyAudio()
    stream = audio.open(format=FORMAT,
                        channels=1,
                        rate=RATE,
                        input=True,
                        frames_per_buffer=CHUNK)
    frames = []
    start_time = time.time()
    while time.time() - start_time < 4:
        data = stream.read(CHUNK, exception_on_overflow=False)
        frames.append(data)
    print("停止录音!")
    stream.stop_stream()
    stream.close()
    audio.terminate()

    audio_data = b''.join(frames)
    audio_base64 = base64.b64encode(audio_data).decode('utf-8')

    host = 'iat-api.xfyun.cn'
    date = time.strftime("%a, %d %b %Y %H:%M:%S GMT", time.gmtime())
    authorization = generate_signature(host, date, API_KEY, API_SECRET)

    url = 'https://iat-api.xfyun.cn/v2/iat'
    headers = {
        'Authorization': authorization,
        'Content-Type': 'application/json',
        'Host': host,
        'Date': date
    }
    payload = {
        "common": {"app_id": APPID},
        "business": {"language": "zh_cn", "domain": "iat", "accent": "mandarin"},
        "data": {
            "status": 2,
            "format": "audio/L16;rate=16000",
            "audio": audio_base64
        }
    }
    response = requests.post(url, headers=headers, data=json.dumps(payload))
    if response.status_code == 200:
        result = response.json()
        if result.get("code") == 0:
            str_ret = ""
            for item in result.get("data").get("result").get("ws"):
                for cw in item.get("cw"):
                    str_ret += cw.get("w")
            str_ret = "".join(str_ret.split())
	 		print(str_ret)
            return str_ret
			
    return ""

