# NVMock: Non-virtual Mock Member Functions

So you're interested in using mocks for your C++ project. Then you find out that in order to mock your favorite type, you need to alter the interface in order to mock member functions which are not virtual. While it may make sense in many cases to refactor into an abstract interface and mock that, it's not always reasonable or even possible. Sometimes you just want to keep a member function non-virtual and be able to mock it.

## Installation

NVMock is a header only library. It depends on boost 1.53+ and is intended to be used with Google Mock. It may be adaptable to other mocking schemes, but I haven't tried.

## Usage

See the example directory in the code for a full use-case.

## Contributing

1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request :D

## Credits

Author: Brandon Kohn

## License

NVMock is released under the Boost license: http://www.boost.org/LICENSE_1_0.txt
