// #include "AnimationWindow.h"
//
// #include <imgui.h>
// #include <ImSequencer.h>
//
// namespace Gleam
// {
//     // struct my_struct : ImSequencer::SequenceInterface
//     // {
//     //     int GetFrameMin() const override { return 0; }
//     //     int GetFrameMax() const override { return 100; }
//     //     int GetItemCount() const override { return 0; }
//     //     void Get(int index, int** start, int** end, int* type, unsigned* color) override
//     //     {
//     //     }
//     // };
//
//     void AnimationWindow::Update()
//     {
//         // static my_struct my_struct;
//         // static bool expanded = true;
//         // static int currentFrame = 100;
//         // static int selectedEntry = -1;
//         // static int firstFrame = 0;
//
//         bool isOpen = true;
//         if (ImGui::Begin("AnimationWindow", &isOpen))
//         {
//             // Sequencer(
//             //     &my_struct, &currentFrame, &expanded, &selectedEntry, &firstFrame,
//             //     ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME
//             // );
//         }
//         ImGui::End();
//
//         if (isOpen == false)
//             World::RemoveSystem(*this);
//     }
// }