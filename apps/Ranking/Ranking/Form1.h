#pragma once

#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

#include "TreeRegressionRanker.h"
#include "LinearRegressionRanker.h"
#include "AdaRanker.h"

using namespace msclr::interop;

// type conversion using string stream
template <typename R, typename T>
R stream_cast(const T &t)
{
    std::stringstream ss;
    ss << t;
    R result;
    ss >> result;
    return result;
}

// querys and rankers
namespace Shaobo
{
    QueryData *querys;
    IdleRanker ranker0;
    LinearRegressionRanker ranker1;
    TreeRegressionRanker ranker2(2);
    AdaRanker ranker3;
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
    private: System::Windows::Forms::Button^  button2;
    private: System::Windows::Forms::Button^  button3;
    private: System::Windows::Forms::Button^  button4;
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
            System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea3 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
            System::Windows::Forms::DataVisualization::Charting::Legend^  legend3 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
            this->chart1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
            this->button2 = (gcnew System::Windows::Forms::Button());
            this->button3 = (gcnew System::Windows::Forms::Button());
            this->button4 = (gcnew System::Windows::Forms::Button());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->chart1))->BeginInit();
            this->SuspendLayout();
            // 
            // button1
            // 
            this->button1->Location = System::Drawing::Point(47, 340);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(94, 23);
            this->button1->TabIndex = 0;
            this->button1->Text = L"Load File";
            this->button1->UseVisualStyleBackColor = true;
            this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click_1);
            // 
            // openFileDialog1
            // 
            this->openFileDialog1->FileName = L"openFileDialog1";
            // 
            // chart1
            // 
            chartArea3->AxisX->Title = L"Number of Retrieved Documents";
            chartArea3->AxisY->Title = L"Number of Relevant Documents";
            chartArea3->Name = L"ChartArea1";
            this->chart1->ChartAreas->Add(chartArea3);
            legend3->Name = L"Legend1";
            this->chart1->Legends->Add(legend3);
            this->chart1->Location = System::Drawing::Point(12, 12);
            this->chart1->Name = L"chart1";
            this->chart1->Size = System::Drawing::Size(636, 322);
            this->chart1->TabIndex = 1;
            this->chart1->Text = L"chart1";
            // 
            // button2
            // 
            this->button2->Location = System::Drawing::Point(218, 340);
            this->button2->Name = L"button2";
            this->button2->Size = System::Drawing::Size(75, 23);
            this->button2->TabIndex = 2;
            this->button2->Text = L"Run Linear";
            this->button2->UseVisualStyleBackColor = true;
            this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
            // 
            // button3
            // 
            this->button3->Location = System::Drawing::Point(371, 340);
            this->button3->Name = L"button3";
            this->button3->Size = System::Drawing::Size(75, 23);
            this->button3->TabIndex = 3;
            this->button3->Text = L"Run Tree";
            this->button3->UseVisualStyleBackColor = true;
            this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
            // 
            // button4
            // 
            this->button4->Location = System::Drawing::Point(517, 340);
            this->button4->Name = L"button4";
            this->button4->Size = System::Drawing::Size(83, 23);
            this->button4->TabIndex = 4;
            this->button4->Text = L"Run AdaRank";
            this->button4->UseVisualStyleBackColor = true;
            this->button4->Click += gcnew System::EventHandler(this, &Form1::button4_Click);
            // 
            // Form1
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(660, 375);
            this->Controls->Add(this->button4);
            this->Controls->Add(this->button3);
            this->Controls->Add(this->button2);
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
                Shaobo::querys = new QueryData(marshal_as<std::string>(openFileDialog1->FileName));

                this->chart1->Series->Clear();

                Shaobo::ranker0.learn(*Shaobo::querys, AveragePrecisionMetric());
                visualiseRankingResult(Shaobo::ranker0, *Shaobo::querys, "No Ranking  ");
            }
        }

        System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) 
        {
            Shaobo::ranker1.learn(*Shaobo::querys, AveragePrecisionMetric());
            visualiseRankingResult(Shaobo::ranker1, *Shaobo::querys, "Linear Reg.  ");
        }

        System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) 
        {
            Shaobo::ranker2.learn(*Shaobo::querys, AveragePrecisionMetric());
            visualiseRankingResult(Shaobo::ranker2, *Shaobo::querys, "Tree Reg.     ");
        }

        System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) 
        {
            Shaobo::ranker3.learn(*Shaobo::querys, AveragePrecisionMetric());
            visualiseRankingResult(Shaobo::ranker3, *Shaobo::querys, "AdaRank      ");
        }

        void visualiseRankingResult(const Ranker &ranker, const QueryData &data, const std::string seriesName)
        {
            const double MAP = AveragePrecisionMetric().measure(ranker.rank(data));

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

