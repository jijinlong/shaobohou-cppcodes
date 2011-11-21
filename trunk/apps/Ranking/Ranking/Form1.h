#pragma once

#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

#include "TreeRegressionRanker.h"
#include "LinearRegressionRanker.h"
#include "AdaRanker.h"

using namespace msclr::interop;

template <typename R, typename T>
R stream_cast(const T &t)
{
    std::stringstream ss;
    ss << t;
    R result;
    ss >> result;
    return result;
}

namespace Ranking {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    /// <summary>
    /// Summary for Form1
    /// </summary>
    public ref class Form1 : public System::Windows::Forms::Form
    {
    public:
        Form1(void)
        {
            InitializeComponent();
            //
            //TODO: Add the constructor code here
            //
        }

    protected:
        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        ~Form1()
        {
            if (components)
            {
                delete components;
            }
        }
    private: System::Windows::Forms::Button^  button1;
    private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;

    private: System::Windows::Forms::DataVisualization::Charting::Chart^  chart1;
    private: System::ComponentModel::IContainer^  components;


    protected: 

    private:
        /// <summary>
        /// Required designer variable.
        /// </summary>


#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        void InitializeComponent(void)
        {
            System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
            System::Windows::Forms::DataVisualization::Charting::Legend^  legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
            this->chart1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->chart1))->BeginInit();
            this->SuspendLayout();
            // 
            // button1
            // 
            this->button1->Location = System::Drawing::Point(278, 340);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(94, 23);
            this->button1->TabIndex = 0;
            this->button1->Text = L"Load Test File";
            this->button1->UseVisualStyleBackColor = true;
            this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click_1);
            // 
            // openFileDialog1
            // 
            this->openFileDialog1->FileName = L"openFileDialog1";
            // 
            // chart1
            // 
            chartArea1->Name = L"ChartArea1";
            this->chart1->ChartAreas->Add(chartArea1);
            legend1->Name = L"Legend1";
            this->chart1->Legends->Add(legend1);
            this->chart1->Location = System::Drawing::Point(12, 12);
            this->chart1->Name = L"chart1";
            this->chart1->Size = System::Drawing::Size(636, 322);
            this->chart1->TabIndex = 1;
            this->chart1->Text = L"chart1";
            // 
            // Form1
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(660, 375);
            this->Controls->Add(this->chart1);
            this->Controls->Add(this->button1);
            this->Name = L"Form1";
            this->Text = L"Form1";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->chart1))->EndInit();
            this->ResumeLayout(false);

        }
#pragma endregion


        System::Void button1_Click_1(System::Object^  sender, System::EventArgs^  e) 
        {
            // Displays an OpenFileDialog
            OpenFileDialog ^ openFileDialog1 = gcnew OpenFileDialog();
            openFileDialog1->Filter = "Text Files|*.txt";
            openFileDialog1->Title = "Select a Test File";

            // Show the Dialog.
            if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
            {
                QueryData querys(marshal_as<std::string>(openFileDialog1->FileName));

                AveragePrecisionMetric metric;

                IdleRanker ranker0(metric);
                ranker0.learn(querys);

                LinearRegressionRanker ranker1(metric);
                ranker1.learn(querys);

                TreeRegressionRanker ranker2(metric);
                ranker2.learn(querys);
                
                /*
                AdaRanker ranker3(metric);
                ranker3.learn(querys);*/

                this->chart1->Series->Clear();
                visualiseRankingResult(ranker0, querys, "No Ranking  ");
                visualiseRankingResult(ranker1, querys, "Linear Reg.  ");
                visualiseRankingResult(ranker2, querys, "Tree Reg.     ");
//                visualiseRankingResult(ranker3, querys, "AdaRank     ");
            }
        }

        void visualiseRankingResult(const Ranker &ranker, const QueryData &data, const std::string seriesName)
        {
            const double MAP = ranker.metric()->measure(ranker.rank(data));

            System::Windows::Forms::DataVisualization::Charting::Series^  series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
            series1->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Line;
            series1->Name = marshal_as<System::String^>(seriesName + " (" + stream_cast<std::string>(MAP) + ")");

            double relSum = 0;
            RankingList rankings = ranker.rank(data.getAllQuery());
            for(unsigned int i = 0; i < rankings.size(); i++)
            {
                relSum += rankings[i].second->relevance();
                series1->Points->AddXY(i+1, relSum);
            }
            this->chart1->Series->Add(series1);
        }
    };
}

