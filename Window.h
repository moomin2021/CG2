#pragma once
#include "Windows.h"

class Window
{
private:
	// �E�B���h�E�T�C�Y
	const int windowWidth;// --> ��
	const int windowHeight;// -> �c

public:
	//�E�B���h�E�N���X
	WNDCLASSEX w;

	// �E�B���h�E�T�C�Y
	RECT wrc;

	// �E�B���h�E�I�u�W�F�N�g
	HWND hwnd;

	// ���b�Z�[�W
	MSG msg;

	// �R���X�g���N�^
	Window(int width, int height);

	// �f�X�g���N�^
	~Window();

	// �E�B���h�E�̏�����
	void WindowInitialize();

	// �E�B���h�E�̉�����Ԃ�
	int GetWindowWidth();

	// �E�B���h�E�̏c����Ԃ�
	int GetWindowHeight();
};