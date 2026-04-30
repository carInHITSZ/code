#!/usr/bin/env python3
import rospy
from std_msgs.msg import Int32
import edge_tts
import asyncio
from pydub import AudioSegment
from pydub.playback import play
import io

async def speak(text):
    global voice_ok_pub
    communicate = edge_tts.Communicate(text=text, voice="zh-CN-XiaoxiaoNeural", rate="-20%")

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


if __name__ == "__main__":
    rospy.init_node("test_pub")
    voice_ok_pub = rospy.Publisher("voice_ok", Int32, queue_size=1)
    while not rospy.is_shutdown():
        text = input("请输入要播报的文本：")
        asyncio.run(speak(text))

'''
# 安装必要的Python包
pip install edge-tts pydub

# 在Ubuntu系统上还需要安装音频播放依赖
sudo apt-get update
sudo apt-get install -y ffmpeg libavcodec-extra
sudo apt-get install -y python3-pyaudio  # 或者使用portaudio
'''
