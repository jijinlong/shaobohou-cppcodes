#pragma once

#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

#include "LinearRegressionRanker.h"
#include "AdaRanker.h"

using namespace msclr::interop;

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
    private: System::Windows::Forms::ImageList^  imageList1;
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
            this->components = (gcnew System::ComponentModel::Container());
            this->button1 = (gcnew System::Windows::Forms::Button());
            this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
            this->imageList1 = (gcnew System::Windows::Forms::ImageList(this->components));
            this->SuspendLayout();
            // 
            // button1
            // 
            this->button1->Location = System::Drawing::Point(144, 340);
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
            // Form1
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(394, 375);
            this->Controls->Add(this->button1);
            this->Name = L"Form1";
            this->Text = L"Form1";
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

                LinearRegressionRanker ranker1(metric);
                ranker1.learn(querys);

                AdaRanker ranker3(metric);
                ranker3.learn(querys);
            }
        }
    };
}

