#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <cmath> 
#include <iostream> 
#include <fstream>  

#include "json.hpp" 

using json = nlohmann::json;

namespace GOTOEngine
{
    struct CurveKeyframe 
    {
        double time;
        double value;
        double in_tangent;
        double out_tangent;
        int tangent_mode; // 0=broken, 1=unified

        CurveKeyframe(double t = 0.0, double v = 0.0, 
            double in_tan = 0.0, double out_tan = 0.0, 
            int mode = 1)
            : time(t)
            , value(v)
            , in_tangent(in_tan)
            , out_tangent(out_tan)
            , tangent_mode(mode) 
        {
        }
    };

    // AnimationCurve 클래스 정의
    class AnimationCurve {
    public:
        // 기본 생성자
        AnimationCurve() = default;

        // JSON 문자열을 받아 처리할 수 있는 생성자 (편의용)
        AnimationCurve(const std::string& json_string)
        {
            try
            {
                json json_data = json::parse(json_string);
                ParseJsonData(json_data);
            }
            catch (const json::parse_error& e)
            {
                throw std::runtime_error("JSON parse error: " + std::string(e.what()));
            }
        }

        // 특정 시간에서의 커브 값 평가
        double Evaluate(double time) const 
        {
            if (keyframes.empty())
            {
                return 0.0;
            }

            if (keyframes.size() == 1) 
            {
                return keyframes[0].value;
            }

            if (time <= keyframes[0].time) 
            {
                return keyframes[0].value;
            }
            if (time >= keyframes.back().time) 
            {
                return keyframes.back().value;
            }

            for (size_t i = 0; i < keyframes.size() - 1; ++i) 
            {
                const CurveKeyframe& kf0 = keyframes[i];
                const CurveKeyframe& kf1 = keyframes[i + 1];

                if (kf0.time <= time && time <= kf1.time) 
                {
                    double dt = kf1.time - kf0.time;
                    if (dt == 0.0) 
                    {
                        return kf0.value;
                    }

                    double t = (time - kf0.time) / dt;
                    double t2 = t * t;
                    double t3 = t2 * t;

                    double h00 = 2 * t3 - 3 * t2 + 1;
                    double h10 = t3 - 2 * t2 + t;
                    double h01 = -2 * t3 + 3 * t2;
                    double h11 = t3 - t2;

                    double m0 = kf0.out_tangent * dt;
                    double m1 = kf1.in_tangent * dt;

                    return h00 * kf0.value + h10 * m0 + h01 * kf1.value + h11 * m1;
                }
            }
            return 0.0;
        }

        // 키프레임을 직접 추가하는 메서드 (기본 생성자 사용 시 유용)
        void AddKeyframe(double time, double value, double in_tan = 0.0, double out_tan = 0.0, int mode = 1) 
        {
            keyframes.emplace_back(time, value, in_tan, out_tan, mode);
            SortKeyframes();
        }

        const std::vector<CurveKeyframe>& GetKeyframes() const 
        {
            return keyframes;
        }

    private:
        std::vector<CurveKeyframe> keyframes;

        // JSON 데이터 파싱 내부 메서드
        void ParseJsonData(const json& json_data) 
        {
            keyframes.clear();
            if (json_data.contains("keyframes") && json_data["keyframes"].is_array()) 
            {
                for (const auto& kf_json : json_data["keyframes"]) 
                {
                    if (kf_json.is_object()) 
                    {
                        try 
                        {
                            double time = kf_json.at("time").get<double>();
                            double value = kf_json.at("value").get<double>();
                            double in_tan = kf_json.at("in_tangent").get<double>();
                            double out_tan = kf_json.at("out_tangent").get<double>();
                            int tangent_mode = kf_json.at("tangent_mode").get<int>();

                            keyframes.emplace_back(time, value, in_tan, out_tan, tangent_mode);
                        }
                        catch (const json::exception& e) 
                        {
                            throw std::runtime_error("Missing or invalid keyframe data in JSON: " + std::string(e.what()));
                        }
                    }
                    else 
                    {
                        throw std::runtime_error("Invalid keyframe format in JSON: expected object.");
                    }
                }
            }
            else 
            {
                throw std::runtime_error("JSON data must contain a 'keyframes' array.");
            }
            SortKeyframes();
        }

        void SortKeyframes() 
        {
            std::sort(keyframes.begin(), keyframes.end(), [](const CurveKeyframe& a, const CurveKeyframe& b) 
                {
                return a.time < b.time;
                });
        }
    };
}



