[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · **Tiếng Việt** · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## Tổng quan

- **Vector thưa không phải là vector**

	“Chỉ số” của nó không bắt đầu từ 0 và cũng không chạy từ trái sang phải, mà hoạt động giống cách người ta viết số bằng tay: chỉ số đóng vai trò trọng số vị trí, nên nó trải từ dương vô cực xuống âm vô cực. Dĩ nhiên không ngôn ngữ nào có vô cực: trong TypeScript nó là `number`, trong C++ là `int64_t`, trong Rust là `i64`.

	Và vì không phải vector toán học, nó không mang theo phép toán nào. Theo nghĩa đó, nó cũng là một từ điển, và mỗi vị trí thực sự chứa được dữ liệu thuộc bất kỳ kiểu nào.

- **Cách dựng nên một vector thưa**

	Chỉ những vị trí khác giá trị mặc định mới được lưu.

	Vậy hãy lấy vài đối tượng gọi là “phần tử” — mỗi cái là một chỉ số đi kèm một giá trị khác giá trị mặc định — xếp chúng thành một danh sách, thêm giá trị mặc định vào, và thế là xong: một vector thưa. Chỉ số nằm ở đâu cũng vậy, k phần tử nghĩa là O(k) bộ nhớ.

## Thư viện

| Ngôn ngữ | Gói | Phiên bản | Trạng thái | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | đã phát hành | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 2.0.0 | đã phát hành | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | đã phát hành | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*chưa có trên vcpkg hay Conan*

Chỉ cần trỏ CMake vào kho mã nguồn

```cmake
include(FetchContent)

FetchContent_Declare(sparse-vector
  GIT_REPOSITORY https://github.com/Calbona/sparse-vector
  GIT_TAG main
  SOURCE_SUBDIR c++
)
FetchContent_MakeAvailable(sparse-vector)

target_link_libraries(your-target PRIVATE Calbona::sparse-vector)
```

Một bản checkout đặt cạnh dự án của bạn cũng chạy y như vậy, với `add_subdirectory(path/to/sparse-vector/c++)`
Một tiền tố cài đặt cũng xuất ra gói CMake, nên `find_package(sparse-vector)` cũng dùng được

### Rust

```sh
cargo add sparse-vector-rs
```

## Chi tiết ngữ nghĩa

### Giá trị mặc định của ô trống

- Vector thưa được tạo ra cùng một giá trị mặc định

- Giá trị mặc định đó có thể thay thế về sau

- Mọi vị trí đều có giá trị xác định, nên việc đọc luôn có câu trả lời: bất kỳ số nguyên nào cũng trả về một giá trị

### Phần tử bằng giá trị mặc định thì không bao giờ được giữ lại

- Ghi giá trị mặc định vào một vị trí cũng như xoá thứ đang có ở đó

- Thay giá trị mặc định sẽ loại bỏ ngay những phần tử bằng nó

- Chính nguyên tắc này giữ cho cấu trúc thưa

### Sự bằng nhau

- Việc một phần tử có bằng giá trị mặc định hay không theo thông lệ thường ngày của từng ngôn ngữ: `===` trong TypeScript, `operator==` trong C++, `PartialEq` trong Rust

- Những trường hợp trái khoáy:
	- `-0.0` bằng `0.0`
	- `NaN` không bằng chính nó
	- `0`, `'0'`, `false` và `null` là bốn giá trị thuộc bốn kiểu khác nhau
	- `===` so sánh tham chiếu đối tượng, còn `operator==` và `PartialEq` so sánh cấu trúc: hai đối tượng khác nhau nhưng cùng nội dung là một giá trị trong TypeScript và hai giá trị trong C++ và Rust, nên cái đầu giữ chúng lại còn hai cái sau loại bỏ

- Khi cần đến đồng nhất, hãy đưa nó vào chính phép bằng nhau của kiểu; kiểu con trỏ cho bạn điều đó ngay: `operator==` của `std::shared_ptr` so sánh con trỏ, còn `Rc<T>` có thể bọc trong một newtype so sánh bằng `Rc::ptr_eq`. README của C++ và Rust có công thức đó

## API

| Công dụng | TypeScript | C++ | Rust |
| --- | --- | --- | --- |
| Tạo vector thưa (bỏ qua giá trị mặc định) | `new SV_vector()` | `SV_vector()` | `SparseVector::new()` (chỉ `f64`) / `SparseVector::default()` |
| Tạo vector thưa | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::with_default(default)` |
| Lấy giá trị mặc định | `getDefaultValue` | `get_default_value()` | `get_default_value()` |
| Đổi giá trị mặc định | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` |
| Lấy số phần tử | `getElementAmount` | `get_element_amount()` | `get_element_amount()` |
| Lấy chiều có nghĩa | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` |
| Lấy chiều dương | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` |
| Lấy chiều âm | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` |
| Lấy giá trị tại chỉ số | `get(index)` | `get(index)` | `get(index)` |
| Ghi giá trị tại chỉ số | `set(index, value)` | `set(index, value)` | `set(index, value)` |
| Đặt lại giá trị tại chỉ số | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` |
| Đặt lại toàn bộ vector | `resetVector()` | `reset_vector()` | `reset_vector()` |
| Mọi phần tử, chỉ số giảm dần | `elements()` | `elements()` | `elements()` |
| Mọi phần tử, chỉ số tăng dần | `invertedElements()` | `inverted_elements()` | `inverted_elements()` |
| Mọi chỉ số không rỗng, giảm dần | `indexes()` | `indexes()` | `indexes()` |
| Mọi chỉ số không rỗng, tăng dần | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` |
| Mọi giá trị không rỗng, giảm dần | `values()` | `values()` | `values()` |
| Mọi giá trị không rỗng, tăng dần | `invertedValues()` | `inverted_values()` | `inverted_values()` |
| Phần tử thứ (n+1) từ bên trái | `element(n)` | `element(n)` | `element(n)` |
| Chỉ số của phần tử đó | `elementIndex(n)` | `element_index(n)` | `element_index(n)` |
| Giá trị của phần tử đó | `elementValue(n)` | `element_value(n)` | `element_value(n)` |
| Phần tử thứ (n+1) từ bên phải | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` |
| Chỉ số của phần tử đó | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` |
| Giá trị của phần tử đó | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` |
| Chữ số có nghĩa thứ (n+1) từ bên trái | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` |
| Chữ số có nghĩa thứ (n+1) từ bên phải | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` |
| Duyệt | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` |
| Sao chép | `clone()` | hàm dựng sao chép | `clone()` |
| Tạo hàng loạt | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` |

*Trong Rust, việc lấy giá trị và liệt kê đòi hỏi `T: Clone`, còn ghi, đặt lại và đổi giá trị mặc định đòi hỏi `T: PartialEq`; trong C++, tương ứng cần khả năng sao chép và `operator==`*

*Khi vượt ngoài phạm vi, hoặc khi đo một chữ số có nghĩa trên vector rỗng, TypeScript ném `TypeError` / `RangeError`, C++ ném `std::out_of_range`, còn Rust panic*

## Giấy phép

MIT
