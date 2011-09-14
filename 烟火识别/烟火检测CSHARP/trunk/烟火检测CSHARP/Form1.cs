using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;

namespace 烟火检测CSHARP
{
    public partial class Form1 : Form
    {
        delegate void SetTextCallback(string text);
        private string filePath;
        bool isDetecting = true;
        bool isDrawing = true;
        bool isShow = false;
        bool isShowFire = false;
        bool isFire = false;
        int index = 0;
        //private Thread fireDetectThread = new Thread(new ThreadStart(fireDetect));
        public Form1()
        {
            InitializeComponent();
            comboBox1.SelectedIndex = 0;
            this.statusLabel.ForeColor = Color.Green;

            pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;
            CurvePaint cp = new CurvePaint();
            cp.XkeduCount = 10;
            cp.YkeduCount = 10;
            cp.XvalueStrMoveleft = 15;
            pictureBox1.Image = cp.drawCurve(new float[] { 1.0f }, new float[] { 1.0f }, "火灾参数曲线图", "时间（S）", "量(像素）");
        }

        private void readButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "avi文件|*.avi";
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                filePath = openFileDialog.FileName;
                Thread fireDetectThread = new Thread(new ThreadStart(fireDetect));
                fireDetectThread.Start();
                isDetecting = true;
                Thread.Sleep(1000);
                Thread drawLineThread = new Thread(new ThreadStart(drawLine));
                drawLineThread.Start();
                isDrawing = true;
            }
        }
        //烟火检测线程
        private void fireDetect()
        {
            CPPDLL.fireDetect(filePath.ToCharArray());
            isDetecting = false;
        }
        //画曲线图线程
        private void drawLine()
        {
            float[] d = null;
            float[] width = null;
            float[] height = null;
            float[] area = null;
            isFire = false;
            int totalSecond = 0;
            while (isDetecting)
            {
                width = FileOperation.readFromFile("Width.txt");
                height = FileOperation.readFromFile("Height.txt");
                area = FileOperation.readFromFile("Area.txt");
                if (index == 0)
                {
                    d = width;
                }
                else if (index == 1)
                {
                    d = area;
                }
                else if (index == 2)
                {
                    d = height;
                }

                if (width == null || height == null || area == null)
                {
                    Thread.Sleep(2000);
                    continue;
                }

                if (width[width.Length - 1] * height[height.Length - 1] >= 24)
                {
                    isFire = true;
                }
                else
                {
                    isFire = false;
                }

                totalSecond = 0;
                for(int i = 0; i < width.Length-1; i++)
                {
                    if (width[i] * height[i] >= 24)
                    {
                        totalSecond++;
                    }
                }

                if (isFire)
                {
                    if (d != null)
                    {
                        statusLabelSetValue("发现火情！");
                        scaleLabelSetValue(area[area.Length - 1].ToString() + "%");
                        widthLabelSetValue(width[width.Length - 1].ToString()+"像素");
                        heightLabelSetValue(height[height.Length - 1].ToString()+"像素");
                        timeLabelSetValue(totalSecond.ToString()+"秒");
                    }
                }
                else
                {
                    statusLabelSetValue("正常");
                    scaleLabelSetValue("----");
                    widthLabelSetValue("----");
                    heightLabelSetValue("----");
                }

                if (d == null || !isShow)
                {
                    Thread.Sleep(2000);
                    continue;
                }
                float[] month = new float[d.Length];
                for (int i = 0; i < d.Length; i++)
                {
                    month[i] = i + 1;
                }

                //float[] d = new float[13] { 20.5f, 60, 10.8f, 15.6f, 30, 70.9f, 50.3f, 30.7f, 70, 50.4f, 30.8f, 20f, 5.6f };
                pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;
                CurvePaint cp = new CurvePaint();
                cp.XkeduCount = 10;
                cp.YkeduCount = 10;
                cp.XvalueStrMoveleft = 15;
                pictureBox1.Image = cp.drawCurve(month, d, "火灾参数曲线图", "时间", "量（像素）");
                Thread.Sleep(2000);
            }
            isDrawing = false;
        }

        private void getCharButton_Click(object sender, EventArgs e)
        {
            /*
            Curve2D cuv2D = new Curve2D();
            cuv2D.Fit();
            pictureBox1.Image = cuv2D.CreateImage();
             * */

            float [] d = FileOperation.readFromFile("Height.txt");

            float[] month = new float[d.Length];
            for (int i = 0; i < d.Length; i++)
            {
                month[i] = i + 1;
            }

            //float[] d = new float[13] { 20.5f, 60, 10.8f, 15.6f, 30, 70.9f, 50.3f, 30.7f, 70, 50.4f, 30.8f, 20f, 5.6f };
            pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;
            CurvePaint cp = new CurvePaint();
            cp.XkeduCount = 10;
            cp.YkeduCount = 10;
            cp.XvalueStrMoveleft = 15;
            pictureBox1.Image = cp.drawCurve(month, d, "火灾参数曲线图", "时间（S）", "百分比（%）");


        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            index = this.comboBox1.SelectedIndex;
            if (!isDrawing)
            {
                float[] d = null;
                if (index == 0)
                {
                    d = FileOperation.readFromFile("Width.txt");
                }
                else if (index == 1)
                {
                    d = FileOperation.readFromFile("Area.txt");
                }
                else if (index == 2)
                {
                    d = FileOperation.readFromFile("Height.txt");
                }

                if (d == null)
                    return;

                float[] month = new float[d.Length];
                for (int i = 0; i < d.Length; i++)
                {
                    month[i] = i + 1;
                }

                //float[] d = new float[13] { 20.5f, 60, 10.8f, 15.6f, 30, 70.9f, 50.3f, 30.7f, 70, 50.4f, 30.8f, 20f, 5.6f };
                pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;
                CurvePaint cp = new CurvePaint();
                cp.XkeduCount = 10;
                cp.YkeduCount = 10;
                cp.XvalueStrMoveleft = 15;
                pictureBox1.Image = cp.drawCurve(month, d, "火灾参数曲线图", "时间（S）", "量");
            }
        }

        private void showButton_Click(object sender, EventArgs e)
        {
            isShow = !isShow;
            if (isShow)
            {
                showButton.Text = "关闭曲线图";

                index = this.comboBox1.SelectedIndex;
                if (!isDrawing)
                {
                    float[] d = null;
                    if (index == 0)
                    {
                        d = FileOperation.readFromFile("Width.txt");
                    }
                    else if (index == 1)
                    {
                        d = FileOperation.readFromFile("Area.txt");
                    }
                    else if (index == 2)
                    {
                        d = FileOperation.readFromFile("Height.txt");
                    }

                    if (d == null)
                        return;

                    float[] month = new float[d.Length];
                    for (int i = 0; i < d.Length; i++)
                    {
                        month[i] = i + 1;
                    }

                    //float[] d = new float[13] { 20.5f, 60, 10.8f, 15.6f, 30, 70.9f, 50.3f, 30.7f, 70, 50.4f, 30.8f, 20f, 5.6f };
                    pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;
                    CurvePaint cp = new CurvePaint();
                    cp.XkeduCount = 10;
                    cp.YkeduCount = 10;
                    cp.XvalueStrMoveleft = 15;
                    pictureBox1.Image = cp.drawCurve(month, d, "火灾参数曲线图", "时间（S）", "量");
                }

            }
            else
            {
                showButton.Text = "显示曲线图";
                pictureBox1.SizeMode = PictureBoxSizeMode.Zoom;
                CurvePaint cp = new CurvePaint();
                cp.XkeduCount = 10;
                cp.YkeduCount = 10;
                cp.XvalueStrMoveleft = 15;
                pictureBox1.Image = cp.drawCurve(new float[] { 1.0f }, new float[] { 1.0f }, "火灾参数曲线图", "时间（S）", "量(像素）");
            }
        }

        //在线程中设置规模的值
        public void scaleLabelSetValue(string value)
        {
            // InvokeRequired需要比较调用线程ID和创建线程ID
            // 如果它们不相同则返回true
            if (this.scaleLabel.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(scaleLabelSetValue);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                this.scaleLabel.Text = value;
                //this.scaleLabel.PerformStep();
            }
        }
        //在线程中设置宽度的值
        public void widthLabelSetValue(string value)
        {
            // InvokeRequired需要比较调用线程ID和创建线程ID
            // 如果它们不相同则返回true
            if (this.widthLabel.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(widthLabelSetValue);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                this.widthLabel.Text = value;
                //this.scaleLabel.PerformStep();
            }
        }
        //在线程中设置高度的值
        public void heightLabelSetValue(string value)
        {
            // InvokeRequired需要比较调用线程ID和创建线程ID
            // 如果它们不相同则返回true
            if (this.heightLabel.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(heightLabelSetValue);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                this.heightLabel.Text = value;
                //this.scaleLabel.PerformStep();
            }
        }
        //在线程中设置火灾状态的值
        public void statusLabelSetValue(string value)
        {
            // InvokeRequired需要比较调用线程ID和创建线程ID
            // 如果它们不相同则返回true
            if (this.statusLabel.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(statusLabelSetValue);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                this.statusLabel.Text = value;
                if (isFire)
                {
                    this.statusLabel.ForeColor = Color.Red;
                }
                else
                {
                    this.statusLabel.ForeColor = Color.Green;
                }
                //this.scaleLabel.PerformStep();
            }
        }
        //在线程中设置火灾时间的值
        public void timeLabelSetValue(string value)
        {
            // InvokeRequired需要比较调用线程ID和创建线程ID
            // 如果它们不相同则返回true
            if (this.timeLabel.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(timeLabelSetValue);
                this.Invoke(d, new object[] { value });
            }
            else
            {
                this.timeLabel.Text = value;
                //this.scaleLabel.PerformStep();
            }
        }
    }
}
