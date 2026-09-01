#pragma once

class IInputDevice
{
public:
	virtual ~IInputDevice() = default;

	virtual bool GetButton(int code) const = 0;

	virtual float GetAxis(int code) const = 0;

	virtual bool HasAnyInput() const = 0;
};