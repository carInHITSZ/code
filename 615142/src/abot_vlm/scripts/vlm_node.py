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
#from vosk import Model, KaldiRecognizer, SetLogLevel
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
import dashscope
from dashscope.audio.asr import *

import requests
import json


num1 = 0
num2 = 0
dashscope.api_key = "sk-ef3ddbe096a94f6ab12ba2555c0615e1"

doubao_key = 3
question1 = 0

img_path='/home/abot/abot_ws/src/abot_vlm/temp/vl_now.jpg'
#prompt = "你接受到了两张图片，请根据图片输出对应的信息。如果图片中有一个计算式，请计算一下结果并输出一个阿拉伯数字，如果图中有香蕉就输出1,如果图中有苹果就输出2,如果图中有梨子就输出3,如果图中有葡萄就输出4,如果图片中有中文汉字：伍或者陆，那么你只需要输出其对应的阿拉伯数字，如果以上都没有就输出7；接着判断图中是否含有一句中文，如果有在数字之后输出整句话，只加上这句识别出来的中文不加其他的中文。输出的最后一句话一定要包含你需要输出的数字以及可能存在的中文,你只发一遍。', img_path='/home/abot/abot_ws/src/abot_vlm/temp/vl_now.jpg')"
#prompt = "你接收到了两张图片，第一张是原图，第二张是二值化后的图像，请根据图片输出对应的信息。你的原图有三种类型，第一种是只有计算式，第二种是由上方的八个字组成的一句话以及下方的一个图像，图像可能是水果或者汉字，第三种是由上方的八个字组成的一句话以及下方的一个二维码。如果原图是第一种类型，请根据第二张二值化后的图像计算一下计算式结果并只输出一个阿拉伯数字；如果原图是第二种类型，在第二张二值化图像下，根据上方的汉字，你将获得一个汉字句子，在原图中，根据下方的图像，你将获得一个阿拉伯数字，最后将汉字句子和阿拉伯数字合并成一句话输出。其中原图是第二种类型的时候，下方图像是水果的时候，为香蕉则输出阿拉伯数字1，为苹果则输出阿拉伯数字2，为梨子则输出阿拉伯数字3，为葡萄则输出阿拉伯数字4，下方图像是汉字的时候，则输出汉字对应的阿拉伯数字。原图是第三种类型的时候，在第二张二值化图像下，根据上方的汉字，你将获得一个汉字句子，将汉字句子和阿拉伯数字合并成一句话输出。其中原图是第二种类型的时候，下方图像是水果的时候，为香蕉则输出阿拉伯数字1，为苹果则输出阿拉伯数字2，为梨子则输出阿拉伯数字3，为葡萄则输出阿拉伯数字4，下方图像是汉字的时候，则输出汉字对应的阿拉伯数字。原图是第三种类型的时候，在第二张二值化图像下，根据上文的汉字，将汉字句子和阿拉伯数字“-1”合并成一句话输出。"
#prompt = "你接收到了两张图片：第一张是原图，第二张是二值化后的图像，请根据图片输出对应的信息。你的原图有三种类型，第一种是只有计算式；第二种是由上方的八个字组成的一句话以及下方的一个图像，图像可能是水果或者汉字；第三种是由上方的八个字组成的一句话以及下方的一个二维码组成。如果原图是第一种类型，请根据第二张二值化后的图像计算一下计算式结果并只输出一个阿拉伯数字；如果原图是第二种类型，在第二张二值化图像下，根据上方的汉字，你将获得一个汉字句子，在原图中，根据下方的图像，你将获得一个阿拉伯数字，最后将汉字句子和阿拉伯数字合并成一句话输出；原图是第三种类型的时候，在第二张二值化图像下，根据上方的汉字，你将获得一个汉字句子，根据下方的二维码，你将获得一个阿拉伯数字，将汉字句子和阿拉伯数字合并成一句话输出。其中原图是第二种类型的时候，下方图像是水果的时候，为香蕉则输出阿拉伯数字1，为苹果则输出阿拉伯数字2，为梨子则输出阿拉伯数字3，为葡萄则输出阿拉伯数字4，下方图像是汉字的时候，则输出汉字对应的阿拉伯数字，水果对应的阿拉伯数字只会是1或2或3或4,汉字对应的阿拉伯数字只会是5或者6；其中原图是第三种类型的时候，根据上文的汉字，将汉字句子和阿拉伯数字“-1”合并成一句话输出。"
#prompt = "你接收到了两张图片，第一张是原图，第二张是二值化后的图像，请根据图片输出对应的信息。你的原图有三种类型，第一种是只有计算式，第二种是由上方的八个字组成的一句话以及下方的一个图像，图像可能是水果或者汉字，第三种是由上方的八个字组成的一句话以及下方的一个二维码。请根据原图来判断原图属于三种类型中的哪一种类型，特别注意区分下方图像，不要混淆二维码和葡萄的情况。如果原图是第一种类型，请根据第二张二值化后的图像计算一下计算式结果并只输出一个阿拉伯数字。如果原图是第二种类型，在第二张二值化图像下，根据上方的汉字，你将获得一个汉字句子，在原图中，根据下方的图像，你将获得一个阿拉伯数字，其中下方图像是水果的时候，为香蕉则输出阿拉伯数字1，为苹果则输出阿拉伯数字2，为梨子则输出阿拉伯数字3，为葡萄则输出阿拉伯数字4，下方图像是汉字的时候，则输出汉字对应的阿拉伯数字，最后将上方图像获得的汉字句子和下方图像获得的阿拉伯数字合并成一句话输出。原图是第三种类型的时候，在第二张二值化图像下，根据上方的汉字，你将获得一个汉字句子，将上方图像获得的汉字句子和阿拉伯数字“-1”合并成一句话输出。"

prompt = "你接收到了两张图片，第一张是原图，第二张是二值化后的图像，请根据图片输出对应的信息。你的原图有三种类型，第一种是只有计算式，第二种是由上方的八个字组成的一句话以及下方的一个图像，图像可能是水果或者汉字，第三种是由上方的八个字组成的一句话以及下方的一个二维码。注意，如果发现原图下方是一个被黑色方框的图形，那么这个下方的图形是一个二维码且原图像属于三种类型的第三种情况。请根据原图来判断原图属于三种类型中的哪一种类型，特别注意区分下方图像，不要混淆二维码和葡萄的情况。如果原图是第一种类型，请根据第二张二值化后的图像计算一下计算式结果并只输出一个阿拉伯数字。如果原图是第二种类型，在第二张二值化图像下，根据上方的汉字，你将获得一个汉字句子，在原图中，根据下方的图像，你将获得一个阿拉伯数字，其中下方图像是水果的时候，为葡萄则输出阿拉伯数字“1”，为香蕉则输出阿拉伯数字“3”，为苹果则输出阿拉伯数字“5”，为梨子则输出阿拉伯数字“7”，下方图像是汉字的时候，若是汉字伍则输出阿拉伯数字“2”，若是汉字陆则输出阿拉伯数字“8”，最后将上方图像获得的汉字句子和下方图像获得的阿拉伯数字合并成一句话输出。原图是第三种类型的时候，在第二张二值化图像下，根据上方的汉字，你将获得一个汉字句子，将上方图像获得的汉字句子和阿拉伯数字“-1”合并成一句话输出。"

prompt_1 = "你接收到了两张图片，第一张是原图，第二张是二值化后的图像，请根据图片输出我需要的信息。图片中只有一个计算式，请根据第二张二值化后的图像，计算一下计算式结果，并输出该结果，注意只输出一个阿拉伯数字。这个阿拉伯数字一定只可能是1至8中的一个数字。"

prompt_2 = "你接收到了两张图片，第一张是原图，第二张是二值化后的图像，请根据图片输出对应的信息。你的原图有两种类型，第一种是由上方的八个字组成的一句话以及下方的一个图像，图像可能是水果或者汉字，第二种是由上方的八个字组成的一句话以及下方的一个二维码。注意，如果发现原图下方是一个被黑色方框的图形，那么这个下方的图形是一个二维码且原图像属于两种类型的第二种情况。请根据原图来判断原图属于两种类型中的哪一种类型，特别注意区分下方图像，不要混淆二维码和葡萄的情况。如果原图是第一种类型，在第二张二值化图像下，根据上方的汉字，你将获得一个汉字句子，在原图中，根据下方的图像，你将获得一个阿拉伯数字，其中下方图像是水果的时候，为葡萄则输出阿拉伯数字“1”，为香蕉则输出阿拉伯数字“3”，为苹果则输出阿拉伯数字“5”，为梨子则输出阿拉伯数字“7”，下方图像是汉字的时候，若是汉字伍则输出阿拉伯数字“2”，若是汉字陆则输出阿拉伯数字“8”，最后将上方图像获得的汉字句子和下方图像获得的阿拉伯数字合并成一句话输出。原图是第二种类型的时候，在第二张二值化图像下，根据上方的汉字，你将获得一个汉字句子，将上方图像获得的汉字句子和阿拉伯数字“-1”合并成一句话输出。"

#此处逻辑有修正
prompt_ask = "请根据以上的句子，猜测出该句子对应的唯一数字，该数字一定在一和八之间。特别注意，每一句话都能指向该数字。请回答一个阿拉伯数字，请勿回复其他任何内容，只需要回答一次。注意一定不要回复其他任何内容，只回答一个这个阿拉伯数字。"

######################
mic = None
stream = None

class Callback(TranslationRecognizerCallback):
    def __init__(self):
        super().__init__()
        self.translation_result = None  # Initialize translation_result
        self.result = None

    def on_open(self) -> None:
        global mic
        global stream
        print("TranslationRecognizerCallback open.")
        mic = pyaudio.PyAudio()
        stream = mic.open(
            format=pyaudio.paInt16, channels=1, rate=16000, input=True
        )

    def on_close(self) -> None:
        global mic
        global stream
        print("TranslationRecognizerCallback close.")
        stream.stop_stream()
        stream.close()
        mic.terminate()
        stream = None
        mic = None

    def on_event(
        self,
        request_id,
        transcription_result: TranscriptionResult,
        translation_result: TranslationResult,
        usage,
    ) -> None:
        if transcription_result is not None:
            print("transcription: ", transcription_result.text)
            self.result = transcription_result.text

callback = Callback()


translator = TranslationRecognizerChat(
    model="gummy-chat-v1",
    format="pcm",
    sample_rate=16000,
    transcription_enabled=True,
    translation_enabled=True,
    translation_target_languages=["en"],
    callback=callback,
)
######################

#sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8')
wakeup_flag = True

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

'''
async def speak(text):
    global voice_ok_pub
    communicate = edge_tts.Communicate(text=text, voice="zh-CN-XiaoxiaoNeural",rate="+20%")

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
'''

async def speak(text):
    global voice_ok_pub
    # 固定文件路径
    audio_file = "/home/abot/music/speak.wav"
    
    # 直接保存为WAV文件（会自动覆盖）
    communicate = edge_tts.Communicate(text=text, voice="zh-CN-XiaoxiaoNeural", rate="+20%")
    await communicate.save(audio_file)
    
    # 使用系统命令播放
    #os.system(f"aplay {audio_file}")  # Linux系统
    os.system('mplayer %s' % audio_file)
    ok_msg = Int32()
    ok_msg.data = 1
    voice_ok_pub.publish(ok_msg)
    
    rospy.loginfo(f"语音播报完成: {text}")

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
    global voice_pub
    voice_msg = String()
    voice_msg.data = result
    '''
    try:
        os.system('mplayer %s' % "/home/abot/music/比赛开始.wav")
        ok_msg = Int32()
        ok_msg.data = 1
        voice_ok_pub.publish(ok_msg)
    except Exception as e:
        print(f"播放文件 {file_path} 失败: {e}，尝试生成语音。")
        asyncio.run(speak(text))
        rospy.sleep(1)
        os.system('mplayer %s' % "/home/abot/music/比赛开始.wav")
    '''

    voice_pub.publish(voice_msg)
    rospy.sleep(1)
    voice_pub.publish(voice_msg)

    '''
    voice_str = result_process(result)
    text = ""
    text = voice_str
    print(text)
    file_path = os.path.join("/home/abot/music", text+".wav")
    '''

    print("发布结果: ", result)

'''
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
        rospy.sleep(1)
        os.system('mplayer %s' % "/home/abot/music/比赛开始.wav")
        ok_msg = Int32()
        ok_msg.data = 1
        voice_ok_pub.publish(ok_msg)
    except Exception as e:
        print(f"播放文件 {file_path} 失败: {e}，尝试生成语音。")
        asyncio.run(speak(text))
        rospy.sleep(1)
        os.system('mplayer %s' % "/home/abot/music/比赛开始.wav")
    print("发布结果: ", result)
'''

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
    global im_flag,result_pub,result_pub_2,question1
    '''
    这里接收来自话题/usb_cam/image_raw的ROS图像格式的消息,并保存图像,是否拍照用的参数服务器,然后设置参数就行,注意要加命名空间路径
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
        result3 = ""
        result2 = 0
        result1 = ""
        # 调用视觉大模型API
        if API_flag == 0:
            result1 = yi_vision_api()
            result1 = re.findall(r'\d+', result1)
            result2 = np.int32(result1[-1])
        elif API_flag == 1:
            result1 = kimi_vision_api()
            print("result1:", result1)
            # 截取返回结果的最后20个字符
            last_20_chars = result1[-20:]
            # 修改正则表达式以匹配包含负号的整数
            match = re.findall(r'-?\d+', last_20_chars)
            if match:
                number_str = match[-1]
                result2 = np.int32(number_str)
            else:
                result2 = 0
                rospy.logerr("未在最后20个字符中找到有效数字")
            
            # 提取汉字字符
            # 使用正则表达式匹配中文字符
            chinese_chars = re.findall(r'[\u4e00-\u9fff]+', result1)
            if chinese_chars:
                result3 = ''.join(chinese_chars)  # 将所有匹配到的汉字连接成字符串
            else:
                result3 = ""
                rospy.logerr("未在最后20个字符中找到正确汉字")
        elif API_flag == 2:

            
            #result1 = doubao_vision_api(img_path, prompt)
            if (question1 == 0):
                result1 = doubao_vision_api(img_path, prompt_1)
            else:
                result1 = doubao_vision_api(img_path, prompt_2)
            
            
            print("result1:", result1)
            '''
            result1 = doubao_vision_api()
            
            print("result1:", result1)
            '''

            # 截取返回结果的最后20个字符
            last_20_chars = result1[-20:]
            # 修改正则表达式以匹配包含负号的整数
            match = re.findall(r'-?\d+', last_20_chars)
            if match:
                number_str = match[-1]
                result2 = np.int32(number_str)
            else:
                result2 = 0
                rospy.logerr("未在最后20个字符中找到有效数字")
            
            # 提取汉字字符
            # 使用正则表达式匹配中文字符
            if (question1 == 1):
                chinese_chars = re.findall(r'[\u4e00-\u9fff]+', result1)
                if chinese_chars:
                    result3 = ''.join(chinese_chars)  # 将所有匹配到的汉字连接成字符串
                else:
                    result3 = ""
                    rospy.logerr("未在最后20个字符中找到正确汉字")


        print('result2:', result2)
        publish_result(result2)
        if (question1 == 1):
            publish_result_2(result3)
        if (question1 == 0):
            question1 = 1


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
def kimi_vision_api(PROMPT='你接受到了两张图片，请根据图片输出对应的信息。如果图片中有一个计算式，请计算一下结果并输出一个阿拉伯数字。如果图中有香蕉就输出1,如果图中有苹果就输出2,如果图中有梨子就输出3,如果图中有葡萄就输出4,如果图片中有中文汉字：伍或者陆，那么你只需要输出其对应的阿拉伯数字，如果以上都没有就输出7；接着判断图中是否含有一句中文，如果有在数字之后输出整句话,只加上这句识别出来的中文不加其他的中文。输出的最后一句话一定要包含你需要输出的数字以及可能存在的中文,你只发一遍。', img_path='/home/abot/abot_ws/src/abot_vlm/temp/vl_now.jpg'):
    '''
    kimi大模型开放平台，kimi-vision视觉语言多模态大模型API
    '''
    
    client = OpenAI(
        api_key="sk-n7l2AUee80cislEK7Az3rnOB4t6qRXqEWig5ggHxANQWiRdE",
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
        model="kimi-k2-0905-preview",  # 替换为正确的模型名称
        messages=[
            {
                "role": "system",
                "content": "你是一个专业的视觉语言模型，你接受到了两张图片，请根据图片输出对应的信息。如果图片中有一个计算式，请计算一下结果并输出一个阿拉伯数字。如果图中有香蕉就输出1,如果图中有苹果就输出2,如果图中有梨子就输出3,如果图中有葡萄就输出4,如果图片中有中文汉字：伍或者陆，那么你只需要输出其对应的阿拉伯数字，如果以上都没有就输出7；接着判断图中是否含有一句中文，如果有在数字之后输出整句话，只加上这句识别出来的中文不加其他的中文。输出的最后一句话一定要包含你需要输出的数字以及可能存在的中文，,你只发一遍。"
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

def doubao_vision_api(img_path, prompt_text):
    global question1
    """
    Args:
        image_url (str): 图片URL地址
        prompt_text (str): 提问文本
    Returns:
        dict: API响应结果
    """
    # 编码为base64数据
    # 读取一个JPEG图片文件，并将其转换为一个可以在网页中直接嵌入的base64编码的data URI
    # 编码为base64数据
    # 读取一个JPEG图片文件，并将其转换为一个可以在网页中直接嵌入的base64编码的data URI
    image_path_pro = '/home/abot/abot_ws/src/abot_vlm/temp/vl_now_pro.jpg'#增加一张二值化处理的图像
    with open(img_path, 'rb') as image_file:
        image = 'data:image/jpeg;base64,' + base64.b64encode(image_file.read()).decode('utf-8')
    
    with open(image_path_pro, 'rb') as image_file:
        image_pro = 'data:image/jpeg;base64,' + base64.b64encode(image_file.read()).decode('utf-8')

    url = "https://ark.cn-beijing.volces.com/api/v3/chat/completions"
    if (doubao_key==1):
        headers = {
            "Content-Type": "application/json",
            "Authorization": "Bearer 861a290f-acb6-402b-90f3-a10562edee08"
        }
    elif(doubao_key==2):
        headers = {
            "Content-Type": "application/json",
            "Authorization": "Bearer 3f2ce3e6-e8d7-42d4-8bb9-f4cca2527079"
        } 
    elif(doubao_key==3):
        headers = {
            "Content-Type": "application/json",
            "Authorization": "Bearer a374f7d5-bf6b-4a4b-b929-8890ec2edb1a"
        } 


    if (question1==0):
        payload = {
            "model": "doubao-1.5-vision-pro-250328",
            "messages": [
                {
                    "content": [
                        {
                            "image_url": {
                                "url": image
                            },
                            "type": "image_url"
                        },
                        {
                            "image_url": {
                                "url": image_pro
                            },
                            "type": "image_url"
                        },
                        {
                            "text": prompt_text,
                            "type": "text"
                        }
                        
                    ],
                    "role": "user"
                }
            ],
            "max_tokens": 300
        }
    else:
        payload = {
            "model": "doubao-1.5-vision-pro-250328",
            "messages": [
                {
                    "content": [
                        {
                            "image_url": {
                                "url": image
                            },
                            "type": "image_url"
                        },
                        {
                            "image_url": {
                                "url": image_pro
                            },
                            "type": "image_url"
                        },
                        {
                            "text": prompt_text,
                            "type": "text"
                        }
                        
                    ],
                    "role": "user"
                }
            ],
            "max_tokens": 800
        }
    
    response = requests.post(url, headers=headers, data=json.dumps(payload))
    
    if response.status_code == 200:
        # 只返回模型的回答内容
        response_json = response.json()
        # 从响应中提取模型的回答
        answer = response_json['choices'][0]['message']['content']
        answer = str(answer)
        return answer
    else:
        print(f"请求失败，状态码: {response.status_code}")
        print(f"错误信息: {response.text}")
        return None
        
def doubao_vision_api_ask(prompt_text):
    """
    Args:
        prompt_text (str): 提问文本
    Returns:
        dict: API响应结果
    """
    url = "https://ark.cn-beijing.volces.com/api/v3/chat/completions"
    if (doubao_key==1):
        headers = {
            "Content-Type": "application/json",
            "Authorization": "Bearer 861a290f-acb6-402b-90f3-a10562edee08"
        }
    elif(doubao_key==2):
        headers = {
            "Content-Type": "application/json",
            "Authorization": "Bearer 3f2ce3e6-e8d7-42d4-8bb9-f4cca2527079"
        } 
    elif(doubao_key==3):
        headers = {
            "Content-Type": "application/json",
            "Authorization": "Bearer a374f7d5-bf6b-4a4b-b929-8890ec2edb1a"
        } 
    payload = {
        "model": "doubao-1.5-vision-pro-250328",
        "messages": [
            {
                "content": [
                    {
                        "text": prompt_text,
                        "type": "text"
                    }
                    
                ],
                "role": "user"
            }
        ],
        "max_tokens": 600
    }
    
    response = requests.post(url, headers=headers, data=json.dumps(payload))
    
    if response.status_code == 200:
        # 只返回模型的回答内容
        response_json = response.json()
        # 从响应中提取模型的回答
        answer = response_json['choices'][0]['message']['content']
        answer = str(answer)
        return answer
    else:
        print(f"请求失败，状态码: {response.status_code}")
        print(f"错误信息: {response.text}")
        return None





####豆包API调用
'''
def doubao_vision_api(PROMPT='你接受到了两张图片，请根据图片输出对应的信息。如果图片中有一个计算式，请计算一下结果并输出一个阿拉伯数字。如果图中有香蕉就输出1,如果图中有苹果就输出2,如果图中有梨子就输出3,如果图中有葡萄就输出4,如果图片中有中文汉字：伍或者陆，那么你只需要输出其对应的阿拉伯数字，如果以上都没有就输出7；接着判断图中是否含有一句中文，如果有在数字之后输出整句话，只加上这句识别出来的中文不加其他的中文。输出的最后一句话一定要包含你需要输出的数字以及可能存在的中文,你只发一遍。', img_path='/home/abot/abot_ws/src/abot_vlm/temp/vl_now.jpg'):
    
    #豆包大模型开放平台，doubao-vision视觉语言多模态大模型API
    
    client = OpenAI(
        api_key="861a290f-acb6-402b-90f3-a10562edee08",
        base_url="https://ark.cn-beijing.volces.com/api/v3/chat/completions"
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
        model="doubao-1.5-vision-pro-250328",  # 替换为正确的模型名称
        messages=[
            {
                "role": "system",
                "content": "你是一个专业的视觉语言模型，你接受到了两张图片，请根据图片输出对应的信息。如果图片中有一个计算式，请计算一下结果并输出一个阿拉伯数字。如果图中有香蕉就输出1,如果图中有苹果就输出2,如果图中有梨子就输出3,如果图中有葡萄就输出4,如果图片中有中文汉字：伍或者陆，那么你只需要输出其对应的阿拉伯数字，如果以上都没有就输出7；接着判断图中是否含有一句中文，如果有在数字之后输出整句话，只加上这句识别出来的中文不加其他的中文。输出的最后一句话一定要包含你需要输出的数字以及可能存在的中文,你只发一遍。"
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
'''

'''
####豆包API调用
def doubao_vision_api(PROMPT='你接受到了两张图片，第一张是原图通过原图来判断图中是否有二维码，如果图中有二维码则你只需要输出阿拉伯数字“-1”，其他什么都不要输出；第二张是二值化后的图像。如果图中要你数个数请用图像分割仔细数数（如果图中问你有几个鸡蛋就输出8；问你几个黄瓜就输出8；问你几个向日葵就输出5；问你几个球就输出6；问你几个西瓜就输出7；问你几个棒棒糖就输出6；问你几个星星就输出7；问你几个人就输出5；问你几根香蕉就输出4；问你几只猫就输出3；问你几辆车就输出2；问你几个苹果就输出1）；如果图片中只有一个汉字那么你只需要输出其对应的阿拉伯数字，不要输出其他信息；如果图片中有一个计算式，请计算一下结果。输出的最后一句话一定要包含你计算的结果，且前后不能有多余的数字', img_path='/home/abot/abot_ws/src/abot_vlm/temp/vl_now.jpg'):
    #豆包大模型开放平台，doubao-vision视觉语言多模态大模型API
    client = OpenAI(
        api_key="40ab0238-7c94-432d-b2cd-6bbe28cf2379",
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
'''
def publish_result(result):
    # 发布结果到话题
    result_msg = Int32()
    result_msg.data = result
    result_pub.publish(result_msg)
    rospy.loginfo("发布结果: %s", str(result))

def publish_result_2(result):
    # 发布结果到话题
    result_msg = String()
    result_msg.data = result
    result_pub_2.publish(result_msg)
    rospy.loginfo("发布结果: %s", str(result))


def main_voice_callback(msg):
    global voice_ok_pub
    text = str(msg.data)
    print(text)
    file_path = os.path.join("/home/abot/music", text+".wav")
    print(f"尝试播放文件 {file_path}。")
    if os.path.exists(file_path):
        os.system('mplayer %s' % file_path)
        ok_msg = Int32()
        ok_msg.data = 1
        voice_ok_pub.publish(ok_msg)
    else:
        print("失败，尝试生成语音。")
        asyncio.run(speak(text))

        
def answer_callback(msg):
    global prompt_ask
    text = ""
    text = str(msg.data)
    print(text)
    global im_flag,result_pub,result_pub_2
    result = ""
    answer = 0
    prompt_ask = text + prompt_ask
    # 调用视觉大模型API
    if API_flag == 2:
        result1 = doubao_vision_api_ask(prompt_ask)
        print("result1:", result1)
        # 截取返回结果的最后20个字符
        last_20_chars = result1[-20:]
        # 修改正则表达式以匹配包含负号的整数
        match = re.findall(r'-?\d+', last_20_chars)
        if match:
            number_str = match[-1]
            answer = np.int32(number_str)
        else:
            answer = 0
            rospy.logerr("未在最后20个字符中找到有效数字")   
    print('answer:', answer)
    answer_pub.publish(answer)




def main():
    global im_flag, result_pub, result_pub_2, voice_pub, voice_str, voice_ok_pub,input_1,answer_pub
    
    print ('Please 1 to continue: ')
    input_1 = input()
    #等待键入任意信息
    if (True):
        rospy.init_node('top_view_shot_node', anonymous=True)
        rospy.Subscriber('/usb_cam/image_raw', ROSImage, top_view_shot)
        rospy.Subscriber('/main_voiceWords', String, main_voice_callback)
        rospy.Subscriber("/askquestion", String, answer_callback)
        

        result_pub = rospy.Publisher('/vlm_node/result', Int32, queue_size=10)
        result_pub_2 = rospy.Publisher('/vlm_node/result_2', String, queue_size=30)
        answer_pub = rospy.Publisher('/answer', Int32, queue_size=10)

        voice_pub = rospy.Publisher('/voice_result', String, queue_size=30)
        # 发布识别的语音消息 全局唯一一次
        voice_ok_pub = rospy.Publisher('/voice_ok', Int32, queue_size=10)
        
        
        rospy.loginfo('视觉大模型模块导入成功！')
        rospy.loginfo('准备识别...')
        # 从参数服务器获取im_flag的值
        #im_flag = rospy.get_param('/top_view_shot_node/im_flag', 255)
        voice_str = "start"
        publish_voice_result(voice_str)
        
        rospy.spin()


if __name__ == '__main__':
    main()

