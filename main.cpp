#include "Input.h"
#include "Window.h"
#include "DirectXManager.h"

// Windows�A�v���ŃG���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// �E�B���h�E�̃N���X����
	Window * win = new Window(1280, 720);

	// �E�B���h�E�̏�����
	win->WindowInitialize();

	// DirectX�Ǘ��N���X����
	DirectXManager * directXM = new DirectXManager();

	// DirectX�̏�����
	directXM->DirectXInitialize(win->hwnd);
	// Input�N���X����
	Input * input = new Input();

	// �L�[�{�[�h���̏�����
	input->InputInitialize(win->w, win->hwnd);

	// �`�揉��������
	directXM->DrawInitialize(win->GetWindowWidth(), win->GetWindowHeight());

	// �Q�[�����[�v
	while (true)
	{
		// ���b�Z�[�W������H
		if (PeekMessage(&win->msg, nullptr, 0, 0, PM_REMOVE))
		{
			// �L�[���̓��b�Z�[�W����
			TranslateMessage(&win->msg);

			// �v���V�[�W���Ƀ��b�Z�[�W�𑗂�
			DispatchMessage(&win->msg);
		}

		// �~�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
		if (win->msg.message == WM_QUIT)
		{
			delete win;
			break;
		}

		// DirectX���t���[�������@��������

#pragma region

		// �L�[�{�[�h�̍X�V����
		input->InputUpdate();

		// DirectX���t���[������
		directXM->DirectXUpdate(win->GetWindowWidth(), win->GetWindowHeight());

#pragma endregion

		// DirectX���t���[�������@�����܂�
	}

	return 0;
}