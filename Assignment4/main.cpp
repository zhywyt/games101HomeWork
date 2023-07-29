#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;
int point_num=4;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < point_num) 
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    // TODO: Implement de Casteljau's algorithm
    std::vector<cv::Point2f> recursive_list;
    for (int i = 0; i < control_points.size() - 1; i++)
    {
        recursive_list.push_back(((1 - t)) * (control_points[i]) + (t) * (control_points[i + 1]));
    }
    if(recursive_list.size()==1)
        return recursive_list[0];
    return recursive_bezier(recursive_list,t);
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.
    for(double i=0;i<1.;i+=0.0001){
        auto point=recursive_bezier(control_points,i);
        window.at<cv::Vec3b>(point.y,point.x)[1]=255; 
    }
}
inline float get_distance(const cv::Point2f&p1,const cv::Point2f&p2){
    return (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y);
}
void bezier_with_antialiasing(const std::vector<cv::Point2f> &control_points, cv::Mat &window){
    for (float i = 0; i < 1; i+=0.001f)
    {
        auto point = recursive_bezier(control_points,i);
        
        float minx=std::floor(point.x),miny=std::floor(point.y);
        //calculate the four points's color arount the input point 
        for(int k=0;k<=1;k++)
        {
            for(int j=0;j<=1;j++)
            {
                float distance=get_distance(cv::Point2f(miny+j,minx+k),point);
                std::cout<<distance<<std::endl;
                float color = std::max(static_cast<uchar>((255*(2-distance)/2)),window.at<cv::Vec3b>(miny+j,minx+k)[1]);
                window.at<cv::Vec3b>(miny+j, minx+k)[1] = color;
            }
        }
    }


}
int main(int argc,const char*argv[]) 
{
    if(argc==2&&std::string(argv[1])=="--help"){
        std::cout<<"The first data is use antialiasing or not.\nInput '-a' to use it!\nInput '-' to defualt\n"
        <<"The second data is number of contol point, maybe you should input more than 4"<<std::endl;       
        return 0;
    }
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    bool use_anti =false;
    int key = -1;
    if(argc>=2&&std::string(argv[1])=="-a")use_anti=true;
    if(argc>=3&&std::stoi(argv[2])>4)point_num=std::stoi(argv[2]);

    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }
        std::cout<<"point_num"<<control_points.size()<<std::endl;

        if (control_points.size() == point_num) 
        {   
            std::string filename="bezier_"+std::to_string(point_num)+"num_";
            if(!use_anti){
                filename+="_without_anti";
                naive_bezier(control_points, window);
                bezier(control_points, window);
            }
            else {
                filename+="with_anti";
                bezier_with_antialiasing(control_points,window);
            }
            filename+=".png";
            cv::imshow("Bezier Curve", window);
            cv::imwrite(filename, window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}
