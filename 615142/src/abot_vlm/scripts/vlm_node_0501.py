#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import rospy
import cv2
import numpy as np
from PIL import Image, ImageFont, ImageDraw
import time
from sensor_msgs.msg import Image as ROSImage
from std_msgs.msg import Int32
from std_msgs.msg import String
from API_KEY import *
import json
import openai
from openai import OpenAI
import base64
import sys
import re
import pyaudio
from vosk import Model, KaldiRecognizer, SetLogLevel
from pypinyin import lazy_pinyin
import rospy
from std_msgs.msg import String
import pyttsx3
import asyncio
import edge_tts
import soundfile
import wave
import io
from pydub import AudioSegment
from pydub.playback import play
import os
from funasr import AutoModel
num1 = 0
num2 = 0


#sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
wakeup_flag = True
# 目标运算符及其拼音
target_operators = {
    "加法": "jia fa",
    "减法": "jian fa",
    "乘法": "cheng fa",
    "除法": "chu fa"
}

# 目标数字及其拼音
target_numbers = {
    "11": "shi yi",
    "12": "shi er",
    "13": "shi san",
}

target_numbers_mohu = {
    "13": "si san",
    "12": "si er",
    "11": "si yi",
}

num_mapping = {
    "0": "零",
    "1": "一",
    "2": "二",
    "3": "三",
    "4": "四",
    "5": "五",
    "6": "六",
    "7": "七",
    "8": "八",
    "9": "九"
}
import re
zuobi_operate = [None,"加法","减法","乘法","除法"]
zuobi_number = [None,"11","12","13"]

async def speak(text):
    global voice_ok_pub
    communicate = edge_tts.Communicate(text=text, voice="zh-CN-XiaoxiaoNeural",rate="-20%")

    # 用 edge-tts 获取音频数据流（MP3）
    mp3_data = b""
    async for chunk in communicate.stream():
        if chunk["type"] == "audio":
            mp3_data += chunk["data"]

    # 将音频流（MP3）转换为 WAV 格式
    audio = AudioSegment.from_file(io.BytesIO(mp3_data), format="mp3")
    wav_audio = audio.set_frame_rate(44100).set_channels(2).set_sample_width(2)  # 生成WAV

    # 播放 WAV 格式音频
    play(wav_audio)
    ok_msg = Int32()
    ok_msg.data = 1
    voice_ok_pub.publish(ok_msg)

def replace_numbers_with_chinese(text):
    """
    将字符串中的阿拉伯数字替换为对应的汉字

    :param text: 输入的字符串
    :return: 替换后的字符串
    """
    # 定义匹配阿拉伯数字的正则表达式
    pattern = re.compile(r'\d')
    # 使用 sub 方法进行替换
    def replace(match):
        return num_mapping.get(match.group(0), match.group(0))
    return pattern.sub(replace, text)

def SaveWave(model):
    # 设置音频参数
    FORMAT = pyaudio.paInt16    # 音频流的格式
    RATE = 44100                # 采样率，单位Hz
    CHUNK = 4000                # 单位帧
    print("开始录音...")
    audio = pyaudio.PyAudio()
    stream = audio.open(format=FORMAT,
                        channels=1,
                        rate=RATE,
                        input=True,
                        frames_per_buffer=CHUNK)
    frames = []
    import time
    start_time = time.time()
    # 修改为记录 10 秒内的音频
    while time.time() - start_time < 4:
        data = stream.read(CHUNK, exception_on_overflow=False)
        frames.append(data)
    print("停止录音!")
    stream.stop_stream()
    stream.close()
    audio.terminate()

    rec = KaldiRecognizer(model, RATE)
    rec.SetWords(True)
    str_ret = ""
    for data in frames:
        if rec.AcceptWaveform(data):
            result = json.loads(rec.Result())
            if 'text' in result:
                str_ret += result['text']

    result = json.loads(rec.FinalResult())
    if 'text' in result:
        str_ret += result['text']

    str_ret = "".join(str_ret.split())

    text = str_ret

    print(f"原文本: {text}")
    if text.find("Java") != -1:
        text = text.replace("Java", "加法")
    elif text.find("java") != -1:
        text = text.replace("java", "加法")

    #将所有阿拉伯数字转化为汉字，方便转拼音匹配
    text = replace_numbers_with_chinese(text)

    # 将识别的文本转换为拼音
    input_pinyin = " ".join(lazy_pinyin(text))
    print(f"转换后的拼音: {input_pinyin}")
    # 初始化最小距离和匹配的运算符、数字
    min_operator_distance = float('inf')
    matched_operator = None
    min_number_distance = float('inf')
    matched_number = None
    # 检查输入的拼音是否包含目标运算符的拼音
    #遍历target_operator字典
    for operator, pinyin in target_operators.items():
        if pinyin in input_pinyin:
            matched_operator = operator
            break
    # 检查输入的拼音是否包含目标数字的拼音
    #遍历target_number字典
    for number, pinyin in target_numbers.items():
        if pinyin in input_pinyin:
            matched_number = number
            break
    
    for number, pinyin in target_numbers_mohu.items():
        if pinyin in input_pinyin:
            matched_number = number
            break
    
    all_opertaors = list(target_operators.keys())
    all_numbers = list(target_numbers.keys())

    if matched_operator in all_opertaors and matched_number in all_numbers:
        return matched_operator,matched_number
    elif matched_operator not in all_opertaors and matched_number not in all_numbers:
        return "加法","13"
    elif matched_operator not in all_opertaors and matched_number in all_numbers:
        return "加法",matched_number
    elif matched_operator in all_opertaors and matched_number not in all_numbers:   
        return matched_operator,"13"
"""

def SaveWave(time=4, save_file="test.wav"):
    global model
    CHUNK = 1024
    FORMAT = pyaudio.paInt16
    CHANNELS = 2
    RATE = 16000
    RECORD_SECONDS = time  # 需要录制的时间
    WAVE_OUTPUT_FILENAME = save_file  # 保存的文件名

    p = pyaudio.PyAudio()  # 初始化
    rospy.loginfo("ON")
    if os.path.exists(save_file):
        os.remove(save_file)

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK)  # 创建录音文件
    frames = []

    print(f"RATE: {RATE}, CHUNK: {CHUNK}, RECORD_SECONDS: {RECORD_SECONDS}")  # 添加调试语句

    for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
        data = stream.read(CHUNK)
        frames.append(data)  # 开始录音

    rospy.loginfo("OFF")

    stream.stop_stream()
    stream.close()
    p.terminate()

    wf = wave.open(WAVE_OUTPUT_FILENAME, 'wb')  # 保存
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(p.get_sample_size(FORMAT))
    wf.setframerate(RATE)
    wf.writeframes(b''.join(frames))
    wf.close()

    rospy.loginfo("Starting recognition")
    # 读取音频文件内容
    audio_data, _ = soundfile.read(save_file)
    # 如果音频是多声道的，将其转换为单声道
    if audio_data.ndim > 1:
        audio_data = audio_data.mean(axis=1)

    res = model.generate(input=audio_data)

    result = res[0].get('text','默认值') 
    text = result
    print(f"原文本: {text}")
    if text.find("Java") != -1:
        text = text.replace("Java", "加法")
    elif text.find("java") != -1:
        text = text.replace("java", "加法")

    #将所有阿拉伯数字转化为汉字，方便转拼音匹配
    text = replace_numbers_with_chinese(text)

    # 将识别的文本转换为拼音
    input_pinyin = " ".join(lazy_pinyin(text))
    print(f"转换后的拼音: {input_pinyin}")
    # 初始化最小距离和匹配的运算符、数字
    min_operator_distance = float('inf')
    matched_operator = None
    min_number_distance = float('inf')
    matched_number = None
    # 检查输入的拼音是否包含目标运算符的拼音
    #遍历target_operator字典
    for operator, pinyin in target_operators.items():
        if pinyin in input_pinyin:
            matched_operator = operator
            break
    # 检查输入的拼音是否包含目标数字的拼音
    #遍历target_number字典
    for number, pinyin in target_numbers.items():
        if pinyin in input_pinyin:
            matched_number = number
            break
    
    for number, pinyin in target_numbers_mohu.items():
        if pinyin in input_pinyin:
            matched_number = number
            break
    
    all_opertaors = list(target_operators.keys())
    all_numbers = list(target_numbers.keys())

    if matched_operator in all_opertaors and matched_number in all_numbers:
        return matched_operator,matched_number
    elif matched_operator not in all_opertaors and matched_number not in all_numbers:
        return "加法","13"
    elif matched_operator not in all_opertaors and matched_number in all_numbers:
        return "加法",matched_number
    elif matched_operator in all_opertaors and matched_number not in all_numbers:   
        return matched_operator,"13"
"""

def result_process(result):
    operation = ''
    number_str = ''
    for char in result:
        if char.isdigit():
            number_str += char
        else:
            operation += char
    voice_str = "识别到终点为" + number_str + "点，运算符为" + operation
    return voice_str

def publish_voice_result(result):
    # 发布结果到话题
    
    voice_msg = String()
    voice_msg.data = result
    voice_pub.publish(voice_msg)
    voice_str = result_process(result)
    text = ""
    text = voice_str
    print(text)
    file_path = os.path.join("/home/abot/music", text+".wav")
    try:
        # 尝试播放文件
        print(f"尝试播放文件 {file_path}。")
        os.system(f'mplayer {file_path}')
        ok_msg = Int32()
        ok_msg.data = 1
        voice_ok_pub.publish(ok_msg)
    except Exception as e:
        print(f"播放文件 {file_path} 失败: {e}，尝试生成语音。")
        asyncio.run(speak(text))
    print("发布结果: ", result)

###API_flag 0表示调用零一，1表示调用kimi，2表示调用豆包
API_flag = 2
def imgmsg_to_cv2(img_msg):
    dtype = np.dtype("uint8")  # Hardcode to 8 bits...
    dtype = dtype.newbyteorder('>' if img_msg.is_bigendian else '<')
    image_opencv = np.ndarray(shape=(img_msg.height, img_msg.width, 3), dtype=dtype, buffer=img_msg.data)

    # If the byte order is different between the message and the system.
    if img_msg.is_bigendian == (sys.byteorder == 'little'):
        image_opencv = image_opencv.byteswap().newbyteorder()

    # Convert to BGR if the encoding is not already BGR
    if img_msg.encoding == "rgb8":
        image_opencv = cv2.cvtColor(image_opencv, cv2.COLOR_RGB2BGR)
    elif img_msg.encoding == "mono8":
        image_opencv = cv2.cvtColor(image_opencv, cv2.COLOR_GRAY2BGR)
    elif img_msg.encoding != "bgr8":
        rospy.logerr("Unsupported encoding: %s", img_msg.encoding)
        return None

    return image_opencv

def cv2_to_imgmsg(cv_image):
    img_msg = Image()
    img_msg.height = cv_image.shape[0]
    img_msg.width = cv_image.shape[1]
    img_msg.encoding = "bgr8"
    img_msg.is_bigendian = 0
    img_msg.data = cv_image.tostring()
    img_msg.step = len(img_msg.data) // img_msg.height # That double line is actually integer division, not a comment
    return img_msg

def preprocess_image(image_path, output_path):
    # 读取图像
    img = cv2.imread(image_path)

    # 转为灰度图
    gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # 二值化处理
    _, binary_img = cv2.threshold(gray_img, 110, 255, cv2.THRESH_BINARY)

    # 取反图像，将黑色笔画变为白色
    inverted_img = cv2.bitwise_not(binary_img)

    # 定义闭操作的核，这里使用 (3, 3)
    close_kernel = np.ones((3, 3), np.uint8)
    # 进行闭操作
    closed_img = cv2.morphologyEx(inverted_img, cv2.MORPH_CLOSE, close_kernel)

    # 定义膨胀核，这里使用 (2, 2)识别到终点是13点，运算符是加法
    kernel = np.ones((3, 3), np.uint8)
    # 进行膨胀操作
    dilated_img = cv2.dilate(closed_img, kernel, iterations=1)

    # 再次取反，恢复原始颜色
    final_img = cv2.bitwise_not(dilated_img)

    # 保存处理后的图像
    cv2.imwrite(output_path, final_img)
    cv2.imshow("Original Image", img)
    cv2.imshow("Final Image", final_img)
    print(f"处理后的图像已保存至: {output_path}")

def top_view_shot(image_msg):
    global im_flag,result_pub
    '''
    这里接收来自话题/usb_cam/image_raw的ROS图像格式的消息，并保存图像，是否拍照用的参数服务器，然后设置参数就行，注意要加命名空间路径
    '''
    # 将ROS图像消息转换为OpenCV格式
    img_bgr = imgmsg_to_cv2(image_msg)
    # 从参数服务器获取im_flag的值，这里默认参数是255，一定要记得设置，加一个保险
    im_flag = rospy.get_param('/top_view_shot_node/im_flag', 255)
    
    if im_flag == 1:
        # 保存图像
        rospy.loginfo('保存至temp/vl_now.jpg')
        cv2.imwrite('/home/abot/abot_ws/src/abot_vlm/temp/vl_now.jpg', img_bgr)
        #opencv处理并替换原图像
        preprocess_image('/home/abot/abot_ws/src/abot_vlm/temp/vl_now.jpg', '/home/abot/abot_ws/src/abot_vlm/temp/vl_now_pro.jpg')
        # 将im_flag重置为255
        rospy.set_param('/top_view_shot_node/im_flag', 255)
        # # 屏幕上展示图像
        #cv2.imshow('vlm', img_bgr)
        cv2.waitKey(1)
        result2 = 0
        result1 = ""
        # 调用视觉大模型API
        if API_flag == 0:
            result1 = yi_vision_api()
            result1 = re.findall(r'\d+', result1)
            result2 = np.int32(result1[-1])
        elif API_flag == 1:
            result1 = kimi_vision_api()
            print("result1:",result1)
            # 截取返回结果的最后20个字符
            last_20_chars = result1[-20:]
            match = re.findall(r'\d+', last_20_chars)
            if match:
                number_str = match[-1]
                result2 = np.int32(number_str)
            else:
                rospy.logerr("未在最后20个字符中找到有效数字")
        elif API_flag == 2:
            result1 = doubao_vision_api()
            print("result1:",result1)
            # 截取返回结果的最后20个字符
            last_20_chars = result1[-20:]
            match = re.findall(r'\d+', last_20_chars)
            if match:
                number_str = match[-1]
                result2 = np.int32(number_str)
            else:
                result2 = 0
                rospy.logerr("未在最后20个字符中找到有效数字")
        print('result2:', result2)
        publish_result(result2)

def yi_vision_api(PROMPT='你接受到了两张图片，第一张是原图通过原图来判断图中是否有二维码，如果图中有二维码则你只需要输出阿拉伯数字“-1”，其他什么都不要输出；如果图片中是一个汉字那么你只需要输出数字“0”，其他什么都不要输出；如果图片中有一个计算式，请计算一下结果并输出，例如：图中内容为1+1=，你输出为2。图中的内容为2+2=，你输出4。注意，你只输出结果，比如数字2,即最后的输出一定是一个数字，除了数字一定不要展示其他内容,我只要输出的数字格式为单个字符 例如X=8，在终端输出的格式为“结果：最终的数字”', img_path='/home/abot/abot_ws/src/abot_vlm/temp/vl_now.jpg'):
    '''
    零一万物大模型开放平台，yi-vision视觉语言多模态大模型API
    '''
    
    client = OpenAI(
        api_key=YI_KEY,
        base_url="https://api.lingyiwanwu.com/v1"
    )
    
    # 编码为base64数据
    # 读取一个JPEG图片文件，并将其转换为一个可以在网页中直接嵌入的base64编码的data URI
    image_path_pro = '/home/abot/abot_ws/src/abot_vlm/temp/vl_now_pro.jpg'#增加一张二值化处理的图像
    with open(img_path, 'rb') as image_file:
        image = 'data:image/jpeg;base64,' + base64.b64encode(image_file.read()).decode('utf-8')
    
    with open(image_path_pro, 'rb') as image_file:
        image_pro = 'data:image/jpeg;base64,' + base64.b64encode(image_file.read()).decode('utf-8')
    
    # 向大模型发起请求
    completion = client.chat.completions.create(
      model="yi-vision",
      messages=[
        {
          "role": "user",
          "content": [
            {
              "type": "text",
              "text": PROMPT
            },
            {
              "type": "image_url",
              "image_url": {
                "url": image
              }
            },
            {
              "type": "image_url",
              "image_url": {
                "url": image_pro
              }
            }
          ]
        },
      ]
    )
    
    # 解析大模型返回结果
    result_str = completion.choices[0].message.content.strip()
    #result = str(result_str)
    print('零一大模型调用成功！')
            
    return result_str

####kimiAPI调用
def kimi_vision_api(PROMPT='你接受到了两张图片，第一张是原图通过原图来判断图中是否有二维码，如果图中有二维码则你只需要输出阿拉伯数字-1”，其他什么都不要输出；如果图中的是二维码，那么你只需要输出阿拉伯数字“-1”，其他什么都不要输出；第二张是二值化后的图像。如果图片中是一个汉字那么你只需要输出其对应的阿拉伯数字，不要输出其他信息；如果图片中有一个计算式或者一个问题，请计算一下结果。输出的最后一句话一定要包含你计算的结果，且前后不能有多余的数字', img_path='/home/abot/abot_ws/src/abot_vlm/temp/vl_now.jpg'):
    '''
    kimi大模型开放平台，kimi-vision视觉语言多模态大模型API
    '''
    
    client = OpenAI(
        api_key="sk-X8J75intvjYbZLdQ939KmF4zJyj0N5E8lzmPlBqyfViyVVSh",
        base_url="https://api.moonshot.cn/v1"
    )
    
    # 编码为base64数据
    # 读取一个JPEG图片文件，并将其转换为一个可以在网页中直接嵌入的base64编码的data URI
    image_path_pro = '/home/abot/abot_ws/src/abot_vlm/temp/vl_now_pro.jpg'#增加一张二值化处理的图像
    with open(img_path, 'rb') as image_file:
        image = 'data:image/jpeg;base64,' + base64.b64encode(image_file.read()).decode('utf-8')
    
    with open(image_path_pro, 'rb') as image_file:
        image_pro = 'data:image/jpeg;base64,' + base64.b64encode(image_file.read()).decode('utf-8')
    
    # 修改此处，将字典参数转换为关键字参数
    completion = client.chat.completions.create(
        model="moonshot-v1-8k-vision-preview",  # 替换为正确的模型名称
        messages=[
            {
                "role": "system",
                "content": "你是一个专业的视觉语言模型，你接受到了两张图片，第一张是原图通过原图来判断图中是否有二维码，如果图中有二维码则你只需要输出阿拉伯数字“-1”，其他什么都不要输出；第二张是二值化后的图像。你可以识别图片中的文字和数字计算题目或者回答问题。如果图中出现汉字，那么你能识别汉字代表的数字是多少，并输出阿拉伯数字是多少；如果图中是一道数学题，你能够进行计算并输出阿拉伯数字结果；如果图中是一道数数问题或者一道需要你输出数字的问题，那么需要你去思考并有一定的逻辑去输出问题的答案对应的阿拉伯数字。"
            },
            {
                "role": "user",
                "content": [
                    {
                        "type": "text",
                        "text": PROMPT
                    },
                    {
                        "type": "image_url",
                        "image_url": {
                            "url": image
                        }
                    },
                    {
                        "type": "image_url",
                        "image_url": {
                            "url": image_pro
                        }
                    }
                ]
            }
        ],
        max_tokens=300
    )
    
    # 解析大模型返回结果
    result_str = completion.choices[0].message.content.strip()
    #result = str(result_str)
    print('kimi大模型调用成功！')
    
    return result_str

####豆包API调用
def doubao_vision_api(PROMPT='你接受到了两张图片，第一张是原图通过原图来判断图中是否有二维码，如果图中有二维码则你只需要输出阿拉伯数字“-1”，其他什么都不要输出；第二张是二值化后的图像。如果图片中是一个汉字那么你只需要输出其对应的阿拉伯数字，不要输出其他信息；如果图片中有一个计算式或者一个问题，请计算一下结果。输出的最后一句话一定要包含你计算的结果，且前后不能有多余的数字', img_path='/home/abot/abot_ws/src/abot_vlm/temp/vl_now.jpg'):
    '''
    豆包大模型开放平台，doubao-vision视觉语言多模态大模型API
    '''
    client = OpenAI(
        api_key="fde44abd-f242-4915-99ae-169478403040",
        base_url="https://ark.cn-beijing.volces.com/api/v3"
    )
    # 编码为base64数据
    # 读取一个JPEG图片文件，并将其转换为一个可以在网页中直接嵌入的base64编码的data URI
    # 编码为base64数据
    # 读取一个JPEG图片文件，并将其转换为一个可以在网页中直接嵌入的base64编码的data URI
    image_path_pro = '/home/abot/abot_ws/src/abot_vlm/temp/vl_now_pro.jpg'#增加一张二值化处理的图像
    with open(img_path, 'rb') as image_file:
        image = 'data:image/jpeg;base64,' + base64.b64encode(image_file.read()).decode('utf-8')
    
    with open(image_path_pro, 'rb') as image_file:
        image_pro = 'data:image/jpeg;base64,' + base64.b64encode(image_file.read()).decode('utf-8')
    # 修改此处，将字典参数转换为关键字参数
    completion = client.chat.completions.create(
        model="doubao-1-5-vision-pro-32k-250115",  # 替换为正确的模型名称
        messages=[
            {
                "role": "system",
                "content": "你是一个专业的视觉语言模型，你接受到了两张图片，第一张是原图通过原图来判断图中是否有二维码，如果图中有二维码则你只需要输出阿拉伯数字“-1”，其他什么都不要输出；第二张是二值化后的图像。你可以识别图片中的文字和数字计算题目或者回答问题。如果图中出现汉字，那么你能识别汉字代表的数字是多少，并输出阿拉伯数字是多少；如果图中是一道数学题，你能够进行计算并输出阿拉伯数字结果；如果图中是一道数数问题或者一道需要你输出数字的问题，那么需要你去思考并有一定的逻辑去输出问题的答案对应的阿拉伯数字。"
            },
            {
                "role": "user",
                "content": [
                    {
                        "type": "text",
                        "text": PROMPT
                    },
                    {
                        "type": "image_url",
                        "image_url": {
                            "url": image
                        }
                    },
                    {
                        "type": "image_url",
                        "image_url": {
                            "url": image_pro
                        }
                    }
                ]
            }
        ],
        max_tokens=600
    )
    # 解析大模型返回结果
    result_str = completion.choices[0].message.content.strip()
    #result = str(result_str)
    print('豆包大模型调用成功！')
    return result_str

def publish_result(result):
    # 发布结果到话题
    result_msg = Int32()
    result_msg.data = result
    result_pub.publish(result_msg)
    rospy.loginfo("发布结果: %s", str(result))

def main_voice_callback(msg):
    global voice_ok_pub
    text = str(msg.data)
    print(text)
    file_path = os.path.join("/home/abot/music", text+".wav")
    try:
        # 尝试播放文件
        print(f"尝试播放文件 {file_path}。")
        os.system(f'mplayer {file_path}')
        ok_msg = Int32()
        ok_msg.data = 1
        voice_ok_pub.publish(ok_msg)
    except Exception as e:
        print(f"播放文件 {file_path} 失败: {e}，尝试生成语音。")
        asyncio.run(speak(text))

def main():
    global im_flag, result_pub, voice_pub, voice_str, voice_ok_pub,model
    model = Model("/home/abot/vosk-model-small-cn-0.22")
    #model = AutoModel(model="/home/abot/paraformer-zh",disable_update=True)
    SetLogLevel(-1)
    print ('Please 1 to continue: ')
    input_1 = input()
    if (True):
        os.system('mplayer %s' % "/home/abot/music/比赛开始.wav")
        rospy.init_node('top_view_shot_node', anonymous=True)
        rospy.Subscriber('/usb_cam/image_raw', ROSImage, top_view_shot)
        rospy.Subscriber('/main_voiceWords', String, main_voice_callback)
        
        result_pub = rospy.Publisher('/vlm_node/result', Int32, queue_size=10)
        voice_pub = rospy.Publisher('/voice_result', String, queue_size=30)
        voice_ok_pub = rospy.Publisher('/voice_ok', Int32, queue_size=10)
        rospy.loginfo('视觉大模型模块导入成功！')
        rospy.loginfo('准备识别...')
        # 从参数服务器获取im_flag的值
        im_flag = rospy.get_param('/top_view_shot_node/im_flag', 255)
        # 语音识别
        #operate = zuobi_operate[num1]
        #number = zuobi_number[num2]
        #print(operate)
        #print(number)
        #if (operate == None and number == None):
        operate, number = SaveWave(model)
        voice_str = operate + number
        print(f"最终识别结果: ", voice_str)
        publish_voice_result(voice_str)
        rospy.spin()

if __name__ == '__main__':
    main()





